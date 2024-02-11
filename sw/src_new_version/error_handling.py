import sys
import logging

# Initialize logging
logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)
handler = logging.StreamHandler()
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
logger.addHandler(handler)

def handle_cleanup():
    """
    Handle cleanup and shutdown procedures.
    """
    logger.info("Initiating cleanup procedures.")

def handle_fatal_error(error_message):
    """
    Handle fatal errors that require the program to exit.
    """
    logger.error(f"Fatal Error: {error_message}")
    handle_cleanup()  # Call the cleanup function before exiting
    sys.exit(1)

def handle_medium_error(error_message):
    """
    Handle medium-severity errors that might not require the program to exit.
    """
    logger.error(f"Medium Error: {error_message}")
    # Implement additional error handling logic as needed

def handle_easy_error(error_message):
    """
    Handle easy errors that require minimal action.
    """
    logger.warning(f"Easy Error: {error_message}")
    # Implement additional error handling logic as needed

# Example of a specific error handling function
def handle_network_error(error_message):
    """
    Handle errors related to network operations.
    """
    logger.error(f"Network Error: {error_message}")
    # Additional network error handling logic
