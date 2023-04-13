#!/usr/bin/env python3

# Tube Converter Aria2 Keeper

# Whenever Aria2 is used for download, a new python process with this script
# is started. Aria2 knows the process' PID and when the process stops Aria2
# stops too. This is used to interrupt download on request.

import time

while True:
    time.sleep(1)
