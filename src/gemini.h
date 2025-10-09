#ifndef GEMINI_H
#define GEMINI_H

// Include API key from config.h (git-ignored)
#include "../config.h"

// Use v1 API with stable model name
#define GEMINI_API_URL "https://generativelanguage.googleapis.com/v1/models/gemini-1.5-flash:generateContent"

// Debug configuration
// Set to 1 to enable debug logging to /tmp/pocketgem_debug.log
#define GEMINI_DEBUG 1

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
