from packet import Packet, Date, Passport, PacketHeader


VERSION = 0x00010000

def generate_packet(
    type: int,
    series: int,
    number: int,
    birth_date: Date,
    claim_date: Date,
    gender: int,
    surname: str,
    name: str,
    lastname: str,
    birth_place: str
) -> Packet:
    header = PacketHeader(VERSION, type)
    passport = Passport(series, number, birth_date, claim_date, gender, surname, name, lastname, birth_place)
    packet = Packet(header, passport)
    return packet


if __name__ == "__main__":
    generate_packet(
        0,
        1234,
        567890,
        Date(12, 12, 1990),
        Date(12, 12, 2020),
        0,
        "Smithy",
        "John",
        "Doe",
        "London"
    ).save("../packet")
