#!/usr/bin/python3

from sseclient import SSEClient
import base64
import struct
import numpy as np
import argparse
import time

def capture_to_file(ip_addr: str, filename: str):
    start_time = time.time()
    with open(filename, "w") as f:
        messages = SSEClient(f"http://{ip_addr}/events", chunk_size=4096)
        for msg in messages:
            data = base64.b64decode(msg.data)
            d = struct.unpack("360h360hfff", data)
            distance = np.array(d[:360])
            intensity = np.array(d[360:2*360])
            distance_print = ",".join([str(x) for x in distance])
            intensity_print = ",".join([str(x) for x in intensity])
            f.write(f"{time.time() - start_time},{distance_print},{intensity_print}\n")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Capture lidar point cloud from lidar board")
    parser.add_argument("--ip", "-i", required=True, help="IP address of the lidar board")
    parser.add_argument("--output", "-o", required=True, help="Path to output file")

    args = parser.parse_args()
    capture_to_file(args.ip, args.output)
