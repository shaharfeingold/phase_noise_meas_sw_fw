#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include "defines.h"
#include "utils_function.h"

// Define the error severity levels 
#define FATAL_ERROR -1
#define MEDIUM_ERROR -2
#define EASY_ERROR -3

// Utility function prototypes
void log_error_to_file(const char* error_message);
void send_error_notification(const char* error_message);
int check_file_exist (char* file_name);

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

int check_file_exist (char* file_name){
    int result = TRUE;
    if (access(file_name, F_OK) != -1) {
       result = FALSE;
    };
    return result;
}

// Utility function to log errors to a file
void log_error_to_file(const char* error_message) {
    if (check_file_exist("error.log")){
        remove("error.log");
    }
    FILE* file = fopen("error.log", "a"); 
    if (file != NULL) {
        fprintf(file, "Error: %s\n", error_message);
        fclose(file);
    } else {
        perror("Failed to open log file");
    }
}

// Utility function to send error notifications (stub)
void send_error_notification(const char* error_message) {

}

