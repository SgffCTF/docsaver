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
    
    def __str__(self):
        return f"{self.day}.{self.month}.{self.year}"

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
    
    
    def __str__(self):
        return f"""{self.surname} {self.name} {self.lastname}
        {self.series} {self.number}
        Gender: {"Male" if self.gender == 0 else "Female"}
        Birth Date: {self.birth_date.__str__()}
        Claim Date: {self.claim_date.__str__()}
        Birth Place: {self.birth_place}"""
        
    
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
    token: str
    
    def __init__(self, header, series, number, token):
        self.header = header
        self.series = series
        self.number = number
        self.token = token
    
    def serialize(self):
        serialized_body = b''
        serialized_body += struct.pack('<H', self.series)
        serialized_body += struct.pack('<I', self.number)
        serialized_body += self.token.encode() + b'\x00'
        
        self.header.length = len(serialized_body)
        self.header.checksum = crc32(serialized_body)
        
        serialized_header = self.header.serialize()
        
        return serialized_header + serialized_body

    def save(self, filename: str):
        with open(filename, 'wb') as file:
            file.write(self.serialize())


def parse_passport(text: bytes):
    series = int(text[:2][::-1].hex(), 16)
    number = int(text[2:6][::-1].hex(), 16)
    birth_date = parse_date(text[6:10])
    claim_date = parse_date(text[10:14])
    gender = text[14]
    shift = 15
    surname, next = parse_string(text[15:])
    shift += next
    name, next = parse_string(text[shift:])
    shift += next
    lastname, next = parse_string(text[shift:])
    shift += next
    birth_place, next = parse_string(text[shift:])
    shift += next
    return Passport(series, number, birth_date, claim_date, gender, surname, name, lastname, birth_place), shift
    
    
def parse_date(text: bytes) -> Date:
    year = int(text[:2][::-1].hex(), 16)
    month = text[2]
    day = text[3]
    return Date(day, month, year)
    

def parse_string(text: bytes):
    null_index = text.find(b"\x00")
    return text[:null_index].decode(), null_index + 1


def generate_send_packet(passport: Passport) -> SendPacket:
    header = PacketHeader(VERSION, 0)
    packet = SendPacket(header, passport)
    return packet


def generate_receive_packet(series: int, number: int, token: str):
    header = PacketHeader(VERSION, 1)
    packet = ReceivePacket(header, series, number, token)
    return packet
