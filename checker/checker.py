#!/usr/bin/env -S python3


import sys
from client import client
from client.packet import generate_random_passport
import random
from checklib import *
from checklib import status


PORT = 16001


class Checker(BaseChecker):
    vulns: int = 1
    timeout: int = 10
    uses_attack_data: bool = True
    
    def __init__(self, *args, **kwargs):
        super(Checker, self).__init__(*args, **kwargs)
        self.client = client.Client(self.host, PORT)
    
    def check(self):
        ping = self.client.ping()
        if not ping:
            self.cquit(Status.DOWN, "Failed to connect")
        
        # === CHECK SEND ===
        passport = generate_random_passport()
        response = self.client.send_passport(passport)
        self.assert_eq(response.status, 0, "Error checking send: " + response.message)
        self.assert_eq(response.message, "Passport send successfully", "Error checking send: " + response.message)
        
        # === CHECK RECEIVE ===
        response = self.client.recv_passport(passport.series, passport.number, response.token)
        self.assert_eq(response.status, 0, "Error checking receive: " + response.message)
        self.assert_eq(response.message, "Passport received successfully", "Error checking receive: " + response.message)
        
        self.cquit(Status.OK)
        
    
    def put(self, flag_id: str, flag: str, vuln: str):
        ping = self.client.ping()
        if not ping:
            self.cquit(Status.DOWN, "Failed to connect")
        
        passport = generate_random_passport(flag)
        response = self.client.send_passport(passport)
        self.assert_eq(response.status, 0, "Error checking send: " + response.message)
        self.assert_eq(response.message, "Passport send successfully", "Error checking send: " + response.message)
        
        private_flag_id = f"{passport.surname}:{passport.series}:{passport.number}:{response.token}"
        public_flag_id = passport.surname
        self.cquit(Status.OK, public=public_flag_id, private=private_flag_id)
    
    def get(self, flag_id: str, flag: str, vuln: str):
        ping = self.client.ping()
        if not ping:
            self.cquit(Status.DOWN, "Failed to connect")
        
        surname, series, number, token = flag_id.split(":")
        
        response = self.client.recv_passport(int(series), int(number), token)
        self.assert_eq(response.status, 0, "Error checking receive: " + response.message)
        self.assert_eq(response.message, "Passport received successfully", "Error checking receive: " + response.message)
        self.assert_eq(response.passport.birth_place, flag, "Incorrect flag")
        
        self.cquit(Status.OK)
    
    
if __name__ == "__main__":
    c = Checker(host=sys.argv[2])
    try:
        c.action(sys.argv[1], *sys.argv[3:])
    except c.get_check_finished_exception() as e:
        cquit(status.Status(c.status), c.public, c.private)
