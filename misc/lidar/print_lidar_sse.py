#!/usr/bin/python3

from sseclient import SSEClient
import base64
import struct
import matplotlib.pyplot as plt
import numpy as np
import math


angle = np.linspace(0, 2 * math.pi, 360)
plt.ion()
fig = plt.figure()
ax = fig.add_subplot(projection='polar')

messages = SSEClient("http://192.168.6.2/events", chunk_size=4096)
for msg in messages:
    data = base64.b64decode(msg.data)
    d = struct.unpack("360h360hfff", data)
    print(d)
    distance = np.array(d[:360])
    intensity = np.array(d[360:2*360])
    reflectivity = intensity * distance**1.0
    print(f"Pose: x={d[-3]}, y={d[-2]}, t={d[-1]}; Refl. max: {max(reflectivity)}")
    ax.clear()
    threshold = 3_500_00
    print(min(reflectivity[reflectivity > threshold]), max(reflectivity))
    ax.scatter(angle[reflectivity > threshold], distance[reflectivity > threshold], c=reflectivity[reflectivity > threshold])
    ax.set_ylim(ymin=0, ymax=4000)
    fig.canvas.draw()
    fig.canvas.flush_events()
