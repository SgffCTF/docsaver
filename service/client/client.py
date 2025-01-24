import struct
from zlib import crc32


VERSION = 0x00010000 # 1.0.0


class Date:
    day: int
    month: int
    year: int
    
    def __init__(self, day, month, year):
        self.day = day
        self.month = month
        self.year = year

class PacketHeader:
    version: int
    type: int # 0 - SEND, 1 - RECEIVE
    length: int
    checksum: int
    
    def __init__(self, version, type):
        self.version = version
        self.type = type
        self.length = 0
        self.checksum = 0
    
    def serialize(self) -> bytes:
        output = b''
        
        output += struct.pack('<I', self.version)
        output += struct.pack('<B', self.type)
        output += struct.pack('<H', self.length)
        output += struct.pack('<I', self.checksum)
        
        return output
    
    def save(self, filename: str):
        with open(filename, 'wb') as file:
            file.write(self.serialize())

class Passport:
    series: int
    number: int
    birth_date: Date
    claim_date: Date
    gender: int # 0 - MALE, 1 - FAMALE
    surname: str
    name: str
    lastname: str
    birth_place: str
    
    def __init__(
        self,
        series,
        number,
        birth_date,
        claim_date,
        gender,
        surname,
        name,
        lastname,
        birth_place
    ):
        self.series = series
        self.number = number
        self.birth_date = birth_date
        self.claim_date = claim_date
        self.gender = gender
        self.surname = surname
        self.name = name
        self.lastname = lastname
        self.birth_place = birth_place
        
    
    def serialize(self) -> bytes:
        output = b''
        
        output += struct.pack('<H', self.series)
        output += struct.pack('<I', self.number)
        output += struct.pack('<H', self.birth_date.year)
        output += struct.pack('<B', self.birth_date.month)
        output += struct.pack('<B', self.birth_date.day)
        output += struct.pack('<H', self.claim_date.year)
        output += struct.pack('<B', self.claim_date.month)
        output += struct.pack('<B', self.claim_date.day)
        output += struct.pack('<B', self.gender)
        output += self.surname.encode('utf-8') + b'\x00'
        output += self.name.encode('utf-8') + b'\x00'
        output += self.lastname.encode('utf-8') + b'\x00'
        output += self.birth_place.encode('utf-8') + b'\x00'
        
        return output

class SendPacket:
    header: PacketHeader
    body: Passport
    
    def __init__(self, header, body):
        self.header = header
        self.body = body
    
    def serialize(self):
        serialized_body = self.body.serialize()        
        
        self.header.length = len(serialized_body)
        self.header.checksum = crc32(serialized_body)
        
        serialized_header = self.header.serialize()
        
        return serialized_header + serialized_body

    def save(self, filename: str):
        with open(filename, 'wb') as file:
            file.write(self.serialize())

class ReceivePacket:
    header: PacketHeader
    series: int
    number: int
    
    def __init__(self, header, series, number):
        self.header = header
        self.series = series
        self.number = number
    
    def serialize(self):
        serialized_body = b''
        serialized_body += struct.pack('<H', self.series)
        serialized_body += struct.pack('<I', self.number)       
        
        self.header.length = len(serialized_body)
        self.header.checksum = crc32(serialized_body)
        
        serialized_header = self.header.serialize()
        
        return serialized_header + serialized_body

    def save(self, filename: str):
        with open(filename, 'wb') as file:
            file.write(self.serialize())


def generate_send_packet(
    series: int,
    number: int,
    birth_date: Date,
    claim_date: Date,
    gender: int,
    surname: str,
    name: str,
    lastname: str,
    birth_place: str
) -> SendPacket:
    header = PacketHeader(VERSION, 0)
    passport = Passport(series, number, birth_date, claim_date, gender, surname, name, lastname, birth_place)
    packet = SendPacket(header, passport)
    return packet


def generate_receive_packet(series: int, number: int):
    header = PacketHeader(VERSION, 1)
    packet = ReceivePacket(header, series, number)
    return packet

def generate_receive_all_packet():
    header = PacketHeader(VERSION, 2)
    return header
