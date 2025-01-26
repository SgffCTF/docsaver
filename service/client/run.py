from client.client import Client
from packet import *


if __name__ == "__main__":
    client = Client("localhost", 16001)
    
    # === SEND === 
    passport = Passport(
        7788,
        123321,
        Date(13, 12, 2005),
        Date(13, 12, 2017),
        1,
        "Ponomarev",
        "Nikita",
        "Viktorovich",
        "Moscow"
    )
    response = client.send_passport(passport)
    print(response.token)
    
    # === RECV ===
    response = client.recv_passport(7788, 123321, response.token)
    print(response.passport)
