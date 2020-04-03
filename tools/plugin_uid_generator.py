#!/usr/bin/python3

import os
uid = int.from_bytes(os.urandom(8), "little")
print(uid, "  ", hex(uid))
