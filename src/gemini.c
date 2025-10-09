#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <glib.h>
#include "gemini.h"

// JSON parsing simple implementation for extracting response
// For a more robust solution, consider using json-glib

typedef struct {
    char *data;
    size_t size;
} MemoryStruct;

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    MemoryStruct *mem = (MemoryStruct *)userp;

    char *ptr = realloc(mem->data, mem->size + realsize + 1);
    if(!ptr) {
        fprintf(stderr, "Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;

    return realsize;
}

// Simple JSON string escaping
static char* escape_json_string(const char *str) {
    const char *p;
    GString *escaped = g_string_new("");

    for (p = str; *p; p++) {
        switch (*p) {
            case '"':  g_string_append(escaped, "\\\""); break;
            case '\\': g_string_append(escaped, "\\\\"); break;
            case '\b': g_string_append(escaped, "\\b"); break;
            case '\f': g_string_append(escaped, "\\f"); break;
            case '\n': g_string_append(escaped, "\\n"); break;
            case '\r': g_string_append(escaped, "\\r"); break;
            case '\t': g_string_append(escaped, "\\t"); break;
            default:
                g_string_append_c(escaped, *p);
        }
    }

    return g_string_free(escaped, FALSE);
}

// Extract text from JSON response (simple parser)
static char* extract_response_text(const char *json) {
    // Look for "text": "..." pattern
    const char *text_start = strstr(json, "\"text\"");
    if (!text_start) {
        return g_strdup("Error: Could not parse response");
    }

    // Find the opening quote of the text value
    const char *value_start = strchr(text_start, ':');
    if (!value_start) {
        return g_strdup("Error: Could not parse response");
    }

    value_start = strchr(value_start, '"');
    if (!value_start) {
        return g_strdup("Error: Could not parse response");
    }
    value_start++; // Skip opening quote

    // Find the closing quote (handle escaped quotes)
    const char *value_end = value_start;
    while (*value_end) {
        if (*value_end == '"' && *(value_end - 1) != '\\') {
            break;
        }
        value_end++;
    }

    // Extract the text
    size_t len = value_end - value_start;
    char *result = g_strndup(value_start, len);

    // Unescape basic JSON escapes
    char *unescaped = g_strcompress(result);
    g_free(result);

    return unescaped;
}

void gemini_init(void) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void gemini_cleanup(void) {
    curl_global_cleanup();
}

char* gemini_send_question(const char *question) {
    CURL *curl;
    CURLcode res;
    MemoryStruct chunk;
    char *response_text = NULL;

    chunk.data = malloc(1);
    chunk.size = 0;

    curl = curl_easy_init();
    if(!curl) {
        free(chunk.data);
        return g_strdup("Error: Failed to initialize CURL");
    }

    // Check if API key is set
    if (strcmp(GEMINI_API_KEY, "YOUR_API_KEY_HERE") == 0) {
        curl_easy_cleanup(curl);
        free(chunk.data);
        return g_strdup("Error: Please set your Gemini API key in src/gemini.h");
    }

    // Build the full URL with API key
    char *url = g_strdup_printf("%s?key=%s", GEMINI_API_URL, GEMINI_API_KEY);

    // Escape the question for JSON
    char *escaped_question = escape_json_string(question);

    // Build JSON request body
    char *json_data = g_strdup_printf(
        "{"
        "\"contents\":[{"
        "\"parts\":[{\"text\":\"%s\"}]"
        "}]"
        "}",
        escaped_question
    );

    g_free(escaped_question);

    // Set up the request
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "pocketGem/1.0");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    // Perform the request
    res = curl_easy_perform(curl);

    if(res != CURLE_OK) {
        response_text = g_strdup_printf("Error: %s", curl_easy_strerror(res));
    } else {
        // Parse the JSON response
        response_text = extract_response_text(chunk.data);
    }

    // Cleanup
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    g_free(url);
    g_free(json_data);
    free(chunk.data);

    return response_text;
}
