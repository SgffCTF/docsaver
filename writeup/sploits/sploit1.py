#!/usr/bin/env python3

import time
import sys
from pwn import *

from client.packet import *


packet = generate_receive_all_packet().serialize()

pr = remote(sys.argv[1], 16001)

pr.sendline(packet)
time.sleep(0.5)
print(pr.recv(), flush=True)

pr.close()