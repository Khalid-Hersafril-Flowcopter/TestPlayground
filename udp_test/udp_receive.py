import socket
import struct
import signal

UDP_IP = "127.0.0.1"
UDP_PORT = 5121

def signal_handler(sig, frame):
    print('Socket closed!')
    sock.close()
    sys.exit(0)

# Attach the signal handler
signal.signal(signal.SIGINT, signal_handler)

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    received_data = struct.unpack('f', data)
    print(received_data)
