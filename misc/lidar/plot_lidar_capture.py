#!/usr/bin/python3

from typing import Generator, List, Tuple
from dataclasses import dataclass
import argparse
import time
import math
import numpy as np
import matplotlib.pyplot as plt

BEACON_POSITIONS = [(0.0, 0.0), (0.0, -1000.0), (1890.0, 0.0)]

@dataclass
class ProcessedFrame:
    points : List[Tuple[float, float, float]]
    pose : Tuple[float, float, float]

def plot_capture(frame_generator: Generator[List[float], None, None]) -> None:
    angle = np.linspace(0, 2 * math.pi, 360)
    plt.ion()
    fig = plt.figure()
    ax = fig.add_subplot(projection='polar')
    for frame in frame_generator:
        distance = np.array(frame[:360])
        intensity = np.array(frame[360:2*360])
        reflectivity = intensity * distance**0.5

        threshold = 200000

        ax.clear()
        ax.scatter(angle, distance, c=(reflectivity > threshold))
        ax.set_ylim(ymin=0, ymax=2000)
        fig.canvas.draw()
        fig.canvas.flush_events()

def plot_cartesian(frame_generator: Generator[ProcessedFrame, None, None]) -> None:
    plt.ion()
    fig = plt.figure()
    ax = fig.add_subplot()
    pose_history = []
    for frame in frame_generator:
        x = np.array(frame.points)[:, 0]
        y = np.array(frame.points)[:, 1]
        c = np.array(frame.points)[:, 2]
        pose_history.append(frame.pose)

        ax.clear()
        ax.scatter(x, y, c=c)
        ax.plot([p[0] for p in pose_history], [p[1] for p in pose_history])
        ax.scatter([b[0] for b in BEACON_POSITIONS], [b[1] for b in BEACON_POSITIONS], marker="s", edgecolors="red", c="w")
        ax.set_ylim(ymin=-2500.0, ymax=1000.0)
        ax.set_xlim(xmin=-400.0, xmax=3000.0)
        fig.canvas.draw()
        fig.canvas.flush_events()

def play_capture_in_real_time(filename: str) -> Generator[List[float], None, None]:
    with open(filename, "r") as f:
        for line in f:
            yield [float(x) for x in line.split(",")]
            time.sleep(0.1)


def caca2(x0, y0, x1, y1, xp0, yp0, xp1, yp1):
    return (
        x0*(xp0*(-xp0 - xp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1) - yp0*(yp0 + yp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1) + (xp0*xp0 + xp1*xp1 + yp0*yp0 + yp1*yp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1)) + x1*(xp1*(-xp0 - xp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1) - yp1*(yp0 + yp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1) + (xp0*xp0 + xp1*xp1 + yp0*yp0 + yp1*yp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1)) + y0*(xp0*(yp0 + yp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1) + yp0*(-xp0 - xp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1)) + y1*(xp1*(yp0 + yp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1) + yp1*(-xp0 - xp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1)),
        x0*(xp0*(-yp0 - yp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1) - yp0*(-xp0 - xp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1)) + x1*(xp1*(-yp0 - yp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1) - yp1*(-xp0 - xp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1)) + y0*(xp0*(-xp0 - xp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1) + yp0*(-yp0 - yp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1) + (xp0*xp0 + xp1*xp1 + yp0*yp0 + yp1*yp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1)) + y1*(xp1*(-xp0 - xp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1) + yp1*(-yp0 - yp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1) + (xp0*xp0 + xp1*xp1 + yp0*yp0 + yp1*yp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1)),
        x0*(4*xp0/(2*xp0*xp0 - 4*xp0*xp1 + 2*xp1*xp1 + 2*yp0*yp0 - 4*yp0*yp1 + 2*yp1*yp1) + (-xp0 - xp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1)) + x1*(4*xp1/(2*xp0*xp0 - 4*xp0*xp1 + 2*xp1*xp1 + 2*yp0*yp0 - 4*yp0*yp1 + 2*yp1*yp1) + (-xp0 - xp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1)) + y0*(4*yp0/(2*xp0*xp0 - 4*xp0*xp1 + 2*xp1*xp1 + 2*yp0*yp0 - 4*yp0*yp1 + 2*yp1*yp1) + (-yp0 - yp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1)) + y1*(4*yp1/(2*xp0*xp0 - 4*xp0*xp1 + 2*xp1*xp1 + 2*yp0*yp0 - 4*yp0*yp1 + 2*yp1*yp1) + (-yp0 - yp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1)),
        x0*(-4*yp0/(2*xp0*xp0 - 4*xp0*xp1 + 2*xp1*xp1 + 2*yp0*yp0 - 4*yp0*yp1 + 2*yp1*yp1) + (yp0 + yp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1)) + x1*(-4*yp1/(2*xp0*xp0 - 4*xp0*xp1 + 2*xp1*xp1 + 2*yp0*yp0 - 4*yp0*yp1 + 2*yp1*yp1) + (yp0 + yp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1)) + y0*(4*xp0/(2*xp0*xp0 - 4*xp0*xp1 + 2*xp1*xp1 + 2*yp0*yp0 - 4*yp0*yp1 + 2*yp1*yp1) + (-xp0 - xp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1)) + y1*(4*xp1/(2*xp0*xp0 - 4*xp0*xp1 + 2*xp1*xp1 + 2*yp0*yp0 - 4*yp0*yp1 + 2*yp1*yp1) + (-xp0 - xp1)/(xp0*xp0 - 2*xp0*xp1 + xp1*xp1 + yp0*yp0 - 2*yp0*yp1 + yp1*yp1))
    )

def caca3(x0, y0, x1, y1, x2, y2, xp0, yp0, xp1, yp1, xp2, yp2):
    return (
        x0*(xp0*(-xp0 - xp1 - xp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2) - yp0*(yp0 + yp1 + yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2) + (xp0*xp0 + xp1*xp1 + xp2*xp2 + yp0*yp0 + yp1*yp1 + yp2*yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + x1*(xp1*(-xp0 - xp1 - xp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2) - yp1*(yp0 + yp1 + yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2) + (xp0*xp0 + xp1*xp1 + xp2*xp2 + yp0*yp0 + yp1*yp1 + yp2*yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + x2*(xp2*(-xp0 - xp1 - xp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2) - yp2*(yp0 + yp1 + yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2) + (xp0*xp0 + xp1*xp1 + xp2*xp2 + yp0*yp0 + yp1*yp1 + yp2*yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + y0*(xp0*(yp0 + yp1 + yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2) + yp0*(-xp0 - xp1 - xp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + y1*(xp1*(yp0 + yp1 + yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2) + yp1*(-xp0 - xp1 - xp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + y2*(xp2*(yp0 + yp1 + yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2) + yp2*(-xp0 - xp1 - xp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)),
        x0*(xp0*(-yp0 - yp1 - yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2) - yp0*(-xp0 - xp1 - xp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + x1*(xp1*(-yp0 - yp1 - yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2) - yp1*(-xp0 - xp1 - xp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + x2*(xp2*(-yp0 - yp1 - yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2) - yp2*(-xp0 - xp1 - xp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + y0*(xp0*(-xp0 - xp1 - xp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2) + yp0*(-yp0 - yp1 - yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2) + (xp0*xp0 + xp1*xp1 + xp2*xp2 + yp0*yp0 + yp1*yp1 + yp2*yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + y1*(xp1*(-xp0 - xp1 - xp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2) + yp1*(-yp0 - yp1 - yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2) + (xp0*xp0 + xp1*xp1 + xp2*xp2 + yp0*yp0 + yp1*yp1 + yp2*yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + y2*(xp2*(-xp0 - xp1 - xp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2) + yp2*(-yp0 - yp1 - yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2) + (xp0*xp0 + xp1*xp1 + xp2*xp2 + yp0*yp0 + yp1*yp1 + yp2*yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)),
        x0*(9*xp0/(6*xp0*xp0 - 6*xp0*xp1 - 6*xp0*xp2 + 6*xp1*xp1 - 6*xp1*xp2 + 6*xp2*xp2 + 6*yp0*yp0 - 6*yp0*yp1 - 6*yp0*yp2 + 6*yp1*yp1 - 6*yp1*yp2 + 6*yp2*yp2) + (-xp0 - xp1 - xp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + x1*(9*xp1/(6*xp0*xp0 - 6*xp0*xp1 - 6*xp0*xp2 + 6*xp1*xp1 - 6*xp1*xp2 + 6*xp2*xp2 + 6*yp0*yp0 - 6*yp0*yp1 - 6*yp0*yp2 + 6*yp1*yp1 - 6*yp1*yp2 + 6*yp2*yp2) + (-xp0 - xp1 - xp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + x2*(9*xp2/(6*xp0*xp0 - 6*xp0*xp1 - 6*xp0*xp2 + 6*xp1*xp1 - 6*xp1*xp2 + 6*xp2*xp2 + 6*yp0*yp0 - 6*yp0*yp1 - 6*yp0*yp2 + 6*yp1*yp1 - 6*yp1*yp2 + 6*yp2*yp2) + (-xp0 - xp1 - xp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + y0*(9*yp0/(6*xp0*xp0 - 6*xp0*xp1 - 6*xp0*xp2 + 6*xp1*xp1 - 6*xp1*xp2 + 6*xp2*xp2 + 6*yp0*yp0 - 6*yp0*yp1 - 6*yp0*yp2 + 6*yp1*yp1 - 6*yp1*yp2 + 6*yp2*yp2) + (-yp0 - yp1 - yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + y1*(9*yp1/(6*xp0*xp0 - 6*xp0*xp1 - 6*xp0*xp2 + 6*xp1*xp1 - 6*xp1*xp2 + 6*xp2*xp2 + 6*yp0*yp0 - 6*yp0*yp1 - 6*yp0*yp2 + 6*yp1*yp1 - 6*yp1*yp2 + 6*yp2*yp2) + (-yp0 - yp1 - yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + y2*(9*yp2/(6*xp0*xp0 - 6*xp0*xp1 - 6*xp0*xp2 + 6*xp1*xp1 - 6*xp1*xp2 + 6*xp2*xp2 + 6*yp0*yp0 - 6*yp0*yp1 - 6*yp0*yp2 + 6*yp1*yp1 - 6*yp1*yp2 + 6*yp2*yp2) + (-yp0 - yp1 - yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)),
        x0*(-9*yp0/(6*xp0*xp0 - 6*xp0*xp1 - 6*xp0*xp2 + 6*xp1*xp1 - 6*xp1*xp2 + 6*xp2*xp2 + 6*yp0*yp0 - 6*yp0*yp1 - 6*yp0*yp2 + 6*yp1*yp1 - 6*yp1*yp2 + 6*yp2*yp2) + (yp0 + yp1 + yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + x1*(-9*yp1/(6*xp0*xp0 - 6*xp0*xp1 - 6*xp0*xp2 + 6*xp1*xp1 - 6*xp1*xp2 + 6*xp2*xp2 + 6*yp0*yp0 - 6*yp0*yp1 - 6*yp0*yp2 + 6*yp1*yp1 - 6*yp1*yp2 + 6*yp2*yp2) + (yp0 + yp1 + yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + x2*(-9*yp2/(6*xp0*xp0 - 6*xp0*xp1 - 6*xp0*xp2 + 6*xp1*xp1 - 6*xp1*xp2 + 6*xp2*xp2 + 6*yp0*yp0 - 6*yp0*yp1 - 6*yp0*yp2 + 6*yp1*yp1 - 6*yp1*yp2 + 6*yp2*yp2) + (yp0 + yp1 + yp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + y0*(9*xp0/(6*xp0*xp0 - 6*xp0*xp1 - 6*xp0*xp2 + 6*xp1*xp1 - 6*xp1*xp2 + 6*xp2*xp2 + 6*yp0*yp0 - 6*yp0*yp1 - 6*yp0*yp2 + 6*yp1*yp1 - 6*yp1*yp2 + 6*yp2*yp2) + (-xp0 - xp1 - xp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + y1*(9*xp1/(6*xp0*xp0 - 6*xp0*xp1 - 6*xp0*xp2 + 6*xp1*xp1 - 6*xp1*xp2 + 6*xp2*xp2 + 6*yp0*yp0 - 6*yp0*yp1 - 6*yp0*yp2 + 6*yp1*yp1 - 6*yp1*yp2 + 6*yp2*yp2) + (-xp0 - xp1 - xp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)) + y2*(9*xp2/(6*xp0*xp0 - 6*xp0*xp1 - 6*xp0*xp2 + 6*xp1*xp1 - 6*xp1*xp2 + 6*xp2*xp2 + 6*yp0*yp0 - 6*yp0*yp1 - 6*yp0*yp2 + 6*yp1*yp1 - 6*yp1*yp2 + 6*yp2*yp2) + (-xp0 - xp1 - xp2)/(2*xp0*xp0 - 2*xp0*xp1 - 2*xp0*xp2 + 2*xp1*xp1 - 2*xp1*xp2 + 2*xp2*xp2 + 2*yp0*yp0 - 2*yp0*yp1 - 2*yp0*yp2 + 2*yp1*yp1 - 2*yp1*yp2 + 2*yp2*yp2)),
    )


def get_pose_from_beacon_positions(beacon_positions: List[Tuple[float, float]], initial_pose: Tuple[float, float, float]) -> Tuple[float, float, float]:
    detected_beacons = [x for x in beacon_positions if not isinstance(x, float)]
    reference_beacons = [BEACON_POSITIONS[i] for i in range(3) if not isinstance(beacon_positions[i], float)]
    if len(detected_beacons) < 2:
        return initial_pose
    elif len(detected_beacons) == 2:
        print("only 2 beacons")
        x0, y0 = reference_beacons[0]
        x1, y1 = reference_beacons[1]
        xp0, yp0 = detected_beacons[0]
        xp1, yp1 = detected_beacons[1]
        mx, my, mcost, msint = caca2(x0, y0, x1, y1, xp0, yp0, xp1, yp1)
        return mx, my, math.atan2(msint, mcost)
    else:
        print("3 beacons")
        x0, y0 = reference_beacons[0]
        x1, y1 = reference_beacons[1]
        x2, y2 = reference_beacons[2]
        xp0, yp0 = detected_beacons[0]
        xp1, yp1 = detected_beacons[1]
        xp2, yp2 = detected_beacons[2]
        mx, my, mcost, msint = caca3(x0, y0, x1, y1, x2, y2, xp0, yp0, xp1, yp1, xp2, yp2)
        return mx, my, math.atan2(msint, mcost)

def slam(frame_generator: Generator[List[float], None, None]) -> Generator[ProcessedFrame, None, None]:
    estimated_pose = 1080.0, -150.0, -0.35
    for frame in frame_generator:
        angle = np.linspace(0, 2 * math.pi, 360)
        distance = np.array(frame[:360])
        intensity = np.array(frame[360:2*360])
        reflectivity = intensity * distance**0.5
        threshold = 180000
        max_acceptable_beacon_error = 200.0

        def find_relative_beacon_points(estimated_pose):
            estimated_points = []
            relative_points = []
            for a, d, r in zip(angle, distance, reflectivity):
                if r < threshold:
                    continue
                xp, yp, tp = estimated_pose
                xe, ye = - (d * math.cos(a - tp) - xp), d * math.sin(a - tp) + yp
                estimated_points.append((xe, ye))

                xr, yr = d * math.cos(a), d * math.sin(a)
                relative_points.append((-xr, yr))
            
            relative_beacon_points = [[] for beacon in BEACON_POSITIONS]
            for estimated, relative in zip(estimated_points, relative_points):
                xe, ye = estimated
                distances_to_beacons = [math.sqrt((beacon_x - xe) ** 2 + (beacon_y - ye) ** 2) for beacon_x, beacon_y in BEACON_POSITIONS]
                if min(distances_to_beacons) > max_acceptable_beacon_error:
                    continue
                beacon_index = np.argmin(distances_to_beacons)
                relative_beacon_points[beacon_index].append(relative)
            
            return relative_beacon_points
        
        relative_beacon_points = find_relative_beacon_points(estimated_pose)
        if sum([rbp == [] for rbp in relative_beacon_points]) >= 2:
            delta_pose = 0.01
            while delta_pose < 0.15:
                print(f"...trying with delta_pose of {delta_pose}")
                x, y, t = estimated_pose
                estimated_pose = x, y, t + delta_pose
                relative_beacon_points = find_relative_beacon_points(estimated_pose)
                if sum([rbp == [] for rbp in relative_beacon_points]) < 2:
                    break
                estimated_pose = x, y, t - delta_pose
                relative_beacon_points = find_relative_beacon_points(estimated_pose)
                if sum([rbp == [] for rbp in relative_beacon_points]) < 2:
                    continue
                delta_pose += 0.01

        measured_relative_beacon_points = [np.average(points_by_beacon, axis=0) for points_by_beacon in relative_beacon_points]

        estimated_pose = get_pose_from_beacon_positions(measured_relative_beacon_points, estimated_pose)
        print(estimated_pose)
      
        aligned_points = []
        for a, d, r in zip(angle, distance, reflectivity):
            xp, yp, tp = estimated_pose
            xe, ye = - (d * math.cos(a - tp) - xp), d * math.sin(a - tp) + yp
            aligned_points.append((xe, ye, r > threshold))

        yield ProcessedFrame(aligned_points, estimated_pose)
            

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Plot captured lidar data")
    parser.add_argument("input", help="Path to CSV capture file")

    args = parser.parse_args()
    #plot_capture(play_capture_in_real_time(args.input))
    plot_cartesian(slam(play_capture_in_real_time(args.input)))
