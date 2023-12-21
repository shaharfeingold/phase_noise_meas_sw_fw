#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// Define the error severity levels 
#define FATAL_ERROR -1
#define MEDIUM_ERROR -2
#define EASY_ERROR -3

// Utility function prototypes
void log_error_to_file(const char* error_message);
void send_error_notification(const char* error_message);

// Handle fatal errors
void handle_fatal_error(const char* error_message) {
    perror(error_message);
    log_error_to_file(error_message);
    exit(EXIT_FAILURE); 
}

// Handle medium-level errors
void handle_medium_error(const char* error_message) {
    perror(error_message);
    log_error_to_file(error_message);
    send_error_notification(error_message);
}

// Handle easy (minor) errors
void handle_easy_error(const char* error_message) {
    perror(error_message);
    log_error_to_file(error_message);
}

// Utility function to log errors to a file
void log_error_to_file(const char* error_message) {
    FILE* file = fopen("", "a"); // Change this path to log path file
    if (file != NULL) {
        fprintf(file, "Error: %s\n", error_message);
        fclose(file);
    } else {
        perror("Failed to open log file");
    }
}

// Utility function to send error notifications (stub)
void send_error_notification(const char* error_message) {
    // Implement sending error notification, e.g., to a monitoring system
    // This is a stub and should be filled in based on your application's needs
}

