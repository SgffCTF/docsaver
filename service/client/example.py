from client import *


if __name__ == "__main__":
    # generate_send_packet(
    #     3456,
    #     567590,
    #     Date(12, 12, 1990),
    #     Date(15, 12, 2008),
    #     0,
    #     "Smith",
    #     "John12345",
    #     "Doe",
    #     "London"
    # ).save("packet")
    # generate_receive_packet(1234, 567890).save("packet")
    packet = generate_receive_all_packet()
    packet.save("packet")