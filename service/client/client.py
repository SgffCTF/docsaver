import time
import socket
from packet import *
from responses import *


class Client:
    s: socket.socket
    
    def __init__(self, host: str, port: int):
        self.connect(host, port)
    
    def connect(self, host: str, port: int):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.connect((host, port))
        
    def send(self, msg: bytes):
        sent = self.s.send(msg)
        if sent == 0:
            raise RuntimeError("socket connection broken")

    def recvuntil(self, until: bytes) -> bytes:
        chunks = b''
        while chunk[len(chunk) - len(until):] != until:
            chunk = self.s.recv(1)
            if chunk == b'':
                raise RuntimeError("socket connection broken")
            chunks += chunk
        return b''.join(chunks)

    def recv(self, count=1024) -> bytes:
        time.sleep(0.1)
        return self.s.recv(count)
    
    def send_passport(self, passport: Passport) -> SendResponse:
        packet = generate_send_packet(passport)
        self.send(packet.serialize())
        
        response = SendResponse()
        response.parse(self.recv())
        
        return response
    
    def recv_passport(self, series: int, number: int, token: str) -> ReceiveResponse:
        packet = generate_receive_packet(series, number, token)
        self.send(packet.serialize())
        
        response = ReceiveResponse()
        response.parse(self.recv())
        
        return response
