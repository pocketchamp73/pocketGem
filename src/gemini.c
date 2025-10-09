#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <curl/curl.h>
#include <glib.h>
#include <time.h>
#include "gemini.h"

// JSON parsing simple implementation for extracting response
// For a more robust solution, consider using json-glib

typedef struct {
    char *data;
    size_t size;
} MemoryStruct;

static FILE *debug_log = NULL;

static void debug_printf(const char *format, ...) {
    #if GEMINI_DEBUG
    if (!debug_log) {
        debug_log = fopen("/tmp/pocketgem_debug.log", "a");
        if (debug_log) {
            time_t now = time(NULL);
            fprintf(debug_log, "\n=== pocketGem Debug Log - %s", ctime(&now));
        }
    }
    if (debug_log) {
        va_list args;
        va_start(args, format);
        vfprintf(debug_log, format, args);
        va_end(args);
        fflush(debug_log);
    }
    #endif
}

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
    const char *text_start;
    const char *value_start;
    const char *value_end;
    size_t len;
    char *result;
    char *unescaped;

    debug_printf("Parsing JSON response:\n%s\n", json);

    // Look for "text": "..." pattern
    text_start = strstr(json, "\"text\"");
    if (!text_start) {
        debug_printf("ERROR: Could not find '\"text\"' in response\n");
        // Check if response contains an error
        if (strstr(json, "\"error\"")) {
            debug_printf("Response contains an error field\n");
            return g_strdup_printf("API Error: %s", json);
        }
        return g_strdup_printf("Error: Could not parse response (no 'text' field found). Raw response logged to /tmp/pocketgem_debug.log");
    }

    // Find the opening quote of the text value
    value_start = strchr(text_start, ':');
    if (!value_start) {
        debug_printf("ERROR: Could not find ':' after 'text'\n");
        return g_strdup("Error: Could not parse response (malformed JSON)");
    }

    value_start = strchr(value_start, '"');
    if (!value_start) {
        debug_printf("ERROR: Could not find opening quote for text value\n");
        return g_strdup("Error: Could not parse response (no text value)");
    }
    value_start++; // Skip opening quote

    // Find the closing quote (handle escaped quotes)
    value_end = value_start;
    while (*value_end) {
        if (*value_end == '"' && *(value_end - 1) != '\\') {
            break;
        }
        value_end++;
    }

    // Extract the text
    len = value_end - value_start;
    result = g_strndup(value_start, len);

    debug_printf("Extracted text (length=%zu): %s\n", len, result);

    // Unescape basic JSON escapes
    unescaped = g_strcompress(result);
    g_free(result);

    return unescaped;
}

void gemini_init(void) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void gemini_cleanup(void) {
    curl_global_cleanup();
    #if GEMINI_DEBUG
    if (debug_log) {
        fclose(debug_log);
        debug_log = NULL;
    }
    #endif
}

char* gemini_send_question(const char *question) {
    CURL *curl;
    CURLcode res;
    MemoryStruct chunk;
    char *response_text = NULL;
    char *url;
    char *escaped_question;
    char *json_data;
    struct curl_slist *headers = NULL;
    long http_code = 0;

    debug_printf("\n--- New Request ---\n");
    debug_printf("Question: %s\n", question);

    chunk.data = malloc(1);
    chunk.size = 0;

    curl = curl_easy_init();
    if(!curl) {
        debug_printf("ERROR: Failed to initialize CURL\n");
        free(chunk.data);
        return g_strdup("Error: Failed to initialize CURL");
    }

    // Check if API key is set
    if (strcmp(GEMINI_API_KEY, "YOUR_API_KEY_HERE") == 0) {
        debug_printf("ERROR: API key not set\n");
        curl_easy_cleanup(curl);
        free(chunk.data);
        return g_strdup("Error: Please set your Gemini API key in src/gemini.h");
    }

    // Build the full URL with API key
    url = g_strdup_printf("%s?key=%s", GEMINI_API_URL, GEMINI_API_KEY);
    debug_printf("URL: %s\n", GEMINI_API_URL); // Don't log API key

    // Escape the question for JSON
    escaped_question = escape_json_string(question);

    // Build JSON request body
    json_data = g_strdup_printf(
        "{"
        "\"contents\":[{"
        "\"parts\":[{\"text\":\"%s\"}]"
        "}]"
        "}",
        escaped_question
    );

    debug_printf("Request JSON: %s\n", json_data);

    g_free(escaped_question);

    // Set up the request
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "pocketGem/1.0");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    // Perform the request
    debug_printf("Sending request to Gemini API...\n");
    res = curl_easy_perform(curl);

    // Get HTTP response code
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    debug_printf("HTTP Response Code: %ld\n", http_code);

    if(res != CURLE_OK) {
        debug_printf("CURL Error: %s\n", curl_easy_strerror(res));
        response_text = g_strdup_printf("Error: %s", curl_easy_strerror(res));
    } else {
        debug_printf("Response received (%zu bytes)\n", chunk.size);
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
