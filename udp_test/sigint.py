import signal
import sys
import time

def signal_handler(sig, frame):
    print('You pressed Ctrl+C!')
    sys.exit(0)

# Attach the signal handler
signal.signal(signal.SIGINT, signal_handler)

# Main program loop
try:
    while True:
        time.sleep(1)  # Sleep for 1 second
except KeyboardInterrupt:
    # Handle Ctrl+C here if needed
    print('Program exiting gracefully')
    sys.exit(0)
