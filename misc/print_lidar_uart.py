import serial
import struct
import matplotlib.pyplot as plt
import numpy as np
import time
import sys
import traceback
import math

START_ADDR = 1073425088 + 720

angle = np.linspace(0, 2 * math.pi, 360)
plt.ion()
fig = plt.figure()
ax = fig.add_subplot(projection='polar')
line, = ax.plot(angle, np.zeros(360), "s")
ax.set_ylim(ymin=0, ymax=4000)
fig.canvas.draw()
fig.canvas.flush_events()

with serial.Serial('/dev/ttyUSB0', 115200, timeout=1) as ser:
    distances = []
    while True:
        s = ser.readline()
        splits = s.split()

        try:
            address = int(splits[3], 16) - START_ADDR
            print(address)
            if address == 0:
                print(len(distances))
                print(distances)
                d = np.array(distances, dtype=np.float_)
                d = np.pad(d, (0, 360 - d.size))
                distances = []
                print(len(d))
                line.set_ydata(d)
                fig.canvas.draw()
                fig.canvas.flush_events()

            b = [x.decode("ascii") for x in splits[4:20]]

            ba = bytearray.fromhex("".join(b))
            distances += struct.unpack("8H", ba)
        except KeyboardInterrupt:
            exit(0)
        except Exception as e:
            traceback.print_exc(file=sys.stdout)
            continue


