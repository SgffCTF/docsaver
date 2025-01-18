from packet import *


VERSION = 0x00010000

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


if __name__ == "__main__":
    # generate_packet(
    #     0,
    #     1234,
    #     567890,
    #     Date(12, 12, 1990),
    #     Date(12, 12, 2020),
    #     0,
    #     "Smithy",
    #     "John",
    #     "Doe",
    #     "London"
    # ).save("packet")
    generate_receive_packet(1234, 567890).save("packet")