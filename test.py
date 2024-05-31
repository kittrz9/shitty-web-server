# this is just something to try and find bugs in the server


import socket

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
	s.connect(("127.0.0.1", 80))
	requestString = b"GET \0"
	print(requestString)
	s.sendall(requestString)
	data = s.recv(2048)

print(data)
