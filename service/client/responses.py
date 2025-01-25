from packet import Passport, parse_passport, parse_string


class SendResponse:
    status: int
    message: str
    token: str
    
    def parse(self, response: bytes):
        self.status = response[0]
        self.message, next = parse_string(response[1:])
        next += 1
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
        next += 1
        if self.status != 0:
            raise Exception("Invalid status: " + self.message)
        self.passport, _ = parse_passport(response[next:])