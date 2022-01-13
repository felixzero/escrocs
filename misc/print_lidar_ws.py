import serial
import struct
import matplotlib.pyplot as plt
import numpy as np
import time
import sys
import math
import websocket

angle = np.linspace(0, 2 * math.pi, 360)
plt.ion()
fig = plt.figure()
ax = fig.add_subplot(projection='polar')

ws = websocket.WebSocket()
ws.connect("ws://192.168.4.2/points")
time.sleep(1)
ws.send_binary("\x00")
time.sleep(1)
while True:
    frame = ws.recv_frame()
    d = struct.unpack("360h360h", frame.data)
    distance = np.array(d[:360])
    intensity = np.array(d[360:])
    reflectivity = intensity * distance
    print(d)
    ax.clear()
    ax.scatter(angle, distance, c=reflectivity)
    ax.set_ylim(ymin=0, ymax=4000)
    fig.canvas.draw()
    fig.canvas.flush_events()

