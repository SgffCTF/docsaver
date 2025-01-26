from client.packet import Passport, parse_string, parse_passport


class SendResponse:
    status: int
    message: str
    token: str
    
    def parse(self, response: bytes):
        self.status = response[0]
        self.message, next = parse_string(response[1:])
        self.token, _ = parse_string(response[next:])
        if self.status != 0:
            raise Exception("Invalid status: " + self.message)


class ReceiveResponse:
    passport: Passport
    status: int
    message: str
    
    def parse(self, response: bytes):
        self.status = response[0]
        self.message, next = parse_string(response[1:])
        if self.status != 0:
            raise Exception("Invalid status: " + self.message)
        self.passport, _ = parse_passport(response[next:])


class ReceiveAllResponse:
    count_passports: int
    passports: list[Passport]
    status: int
    message: str
    
    def parse(self, response: bytes):
        self.status = response[0]
        self.message, next = parse_string(response[1:])
        if self.status != 0:
            raise Exception("Invalid status: " + self.message)
        self.count_passports = int(response[next:next + 4].hex(), 16)
        next += 4
        self.passports = []
        for _ in range(self.count_passports):
            passport, next = parse_passport(response[next:])
            self.passports.append(passport)