#!/usr/bin/env python3
"""mkkmem.py FILE [NMOUNT]: a /dev/kmem image (device: kmem image:FILE) for programs that read the kernel's tables. df reads
the symbol _Nmount of /wega (the segmented address 0x01000032 in /dev/kmem, a 16 bit number: how many mount table entries it
allocates) and then counts the free blocks of the device it is given: this is all of kmem that df -f needs. The file is sparse
(16 MB and a little)."""
import sys

f = open(sys.argv[1], 'wb')
f.truncate(0x01000100)
f.seek(0x01000032)
n = int(sys.argv[2]) if len(sys.argv) > 2 else 8
f.write(bytes([n >> 8, n & 255]))
f.close()
