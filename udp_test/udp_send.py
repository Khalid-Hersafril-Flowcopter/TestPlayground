import socket
import signal
import sys
import time
import struct

def signal_handler(sig, frame):
    print('Socket closed!')
    sock.close()
    sys.exit(0)

UDP_IP = "169.254.94.83"
TARGET_UDP_PORT = 8888
SRC_UDP_PORT = 9002

# Attach the signal handler
signal.signal(signal.SIGINT, signal_handler)

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP

sock.bind((UDP_IP, SRC_UDP_PORT))

print("UDP IP: %s" % UDP_IP)
print("UDP source port: %s" % SRC_UDP_PORT)
print("UDP target port: %s" % TARGET_UDP_PORT)

acceleration = 10.2328974
MESSAGE = struct.pack('f', acceleration)

while True:
    time.sleep(1)
    sock.sendto(MESSAGE, (UDP_IP, TARGET_UDP_PORT))
