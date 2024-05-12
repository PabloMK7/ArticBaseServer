#!/usr/bin/env python
# coding: utf-8 -*-

import os
import socket
import struct
import sys
import threading
import time
import urllib

try:
    from SimpleHTTPServer import SimpleHTTPRequestHandler
    from SocketServer import TCPServer
    from urlparse import urljoin
    from urllib import pathname2url, quote
except ImportError:
    from http.server import SimpleHTTPRequestHandler
    from socketserver import TCPServer
    from urllib.parse import urljoin, quote
    from urllib.request import pathname2url

if len(sys.argv) < 3 or len(sys.argv) > 5:
    print('Usage: ' + sys.argv[0] + ' <target ip> <file / directory> [host ip] [host port]')
    sys.exit(1)

accepted_extension = ('.cia', '.tik', '.cetk')
target_ip = sys.argv[1]
target_path = sys.argv[2]
hostPort = 8080 # Default value

if not os.path.exists(target_path):
    print(target_path + ': No such file or directory.')
    sys.exit(1)

if len(sys.argv) >= 4:
    hostIp = sys.argv[3]
    if len(sys.argv) == 5:
        hostPort = int(sys.argv[4])
else:
    print('Detecting host IP...')
    hostIp = [(s.connect(('8.8.8.8', 53)), s.getsockname()[0], s.close()) for s in [socket.socket(socket.AF_INET, socket.SOCK_DGRAM)]][0][1]

print('Preparing data...')
baseUrl = hostIp + ':' + str(hostPort) + '/'

if os.path.isfile(target_path):
    if target_path.endswith(accepted_extension):
        file_list_payload = baseUrl + quote(os.path.basename(target_path))
        directory = os.path.dirname(target_path)  # get file directory
    else:
        print('Unsupported file extension. Supported extensions are: ' + accepted_extension)
        sys.exit(1)

else:
    directory = target_path  # it's a directory
    file_list_payload = ''  # init the payload before adding lines
    for file in [file for file in next(os.walk(target_path))[2] if file.endswith(accepted_extension)]:
        file_list_payload += baseUrl + quote(file) + '\n'

if len(file_list_payload) == 0:
    print('No files to serve.')
    sys.exit(1)

file_list_payloadBytes = file_list_payload.encode('ascii')

if directory and directory != '.':  # doesn't need to move if it's already the current working directory
    os.chdir(directory)  # set working directory to the right folder to be able to serve files

print('\nURLs:')
print(file_list_payload + '\n')

print('Opening HTTP server on port ' + str(hostPort))
server = TCPServer(('', hostPort), SimpleHTTPRequestHandler)
thread = threading.Thread(target=server.serve_forever)
thread.start()

try:
    print('Sending URL(s) to ' + target_ip + ' on port 5000...')
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((target_ip, 5000))
    sock.sendall(struct.pack('!L', len(file_list_payloadBytes)) + file_list_payloadBytes)
    while len(sock.recv(1)) < 1:
        time.sleep(0.05)
    sock.close()
except Exception as e:
    print('An error occurred: ' + str(e))
    server.shutdown()
    sys.exit(1)

print('Shutting down HTTP server...')
server.shutdown()
