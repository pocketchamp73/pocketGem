#ifndef GEMINI_H
#define GEMINI_H

// Gemini API configuration
// IMPORTANT: Set your API key here before compiling
#define GEMINI_API_KEY "YOUR_API_KEY_HERE"
#define GEMINI_API_URL "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash-latest:generateContent"

/**
 * Send a question to Gemini API and get a response
 * @param question The question to ask
 * @return The response text (caller must free with g_free())
 */
char* gemini_send_question(const char *question);

/**
 * Initialize the Gemini API client
 */
void gemini_init(void);

/**
 * Cleanup Gemini API client resources
 */
void gemini_cleanup(void);

#endif // GEMINI_H
