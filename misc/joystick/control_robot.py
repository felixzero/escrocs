#!/usr/bin/env python3

from typing import Tuple
import pygame
import time
import requests

clock = pygame.time.Clock()
screen = pygame.display.set_mode((200, 200))

pygame.joystick.init()
joystick = pygame.joystick.Joystick(0)
joystick.init()

JOYSTICK_A_BUTTON = 0
JOYSTICK_STEPPER_BUTTONS = 2, 3, 1

JOYSTICK_X_AXIS = 0
JOYSTICK_Y_AXIS = 1
JOYSTICK_THETAP_AXIS = 2
JOYSTICK_THETAM_AXIS = 5
JOYSTICK_STEPPER_AXIS = 4

SAMPLING_RATE = 3.0
SAMPLING_PERIOD = 1.0 / SAMPLING_RATE
DESIRED_SPEED = 60.0
DESIRED_ANGULAR_SPEED = 0.5
DESIRED_STEPPER_INCREMENT = 100

def get_robot_pose() -> Tuple[float, float, float]:
    r = requests.post("http://192.168.6.1/action/get_pose", json={})
    d = r.json()
    return d["x"], d["y"], d["theta"]

def set_robot_target(x: float, y: float, theta: float) -> None:
    print(f"Moving to {x}, {y}, {theta}")
    r = requests.post("http://192.168.6.1/action/set_pose", json={"x": x, "y": y, "theta": theta, "perform_detection": False})

def stop_robot() -> None:
    r = requests.post("http://192.168.6.1/action/stop_motion", json={})

def move_stepper(channel: int, target: float) -> None:
    r = requests.post("http://192.168.6.1/action/move_stepper", json={"channel": channel, "target": target, "speed": 0.2})

for i in range(3):
    requests.post("http://192.168.6.1/action/reset_stepper", json={"channel": i, "value": 0})
stepper_pos = [0, 0, 0]

done = False
while not done:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            done = True

    x, y, theta = get_robot_pose()
    jx, jy = joystick.get_axis(JOYSTICK_X_AXIS), joystick.get_axis(JOYSTICK_Y_AXIS)
    jtp, jtm = joystick.get_axis(JOYSTICK_THETAP_AXIS), joystick.get_axis(JOYSTICK_THETAM_AXIS)
    print(x, y, theta)
    if not joystick.get_button(JOYSTICK_A_BUTTON) or (abs(jx) < 0.3 and abs(jy) < 0.3 and jtp < -0.8 and jtm < -0.8):
        stop_robot()
    else:
        delta_x = jx * DESIRED_SPEED / SAMPLING_PERIOD
        delta_y = jy * DESIRED_SPEED / SAMPLING_PERIOD
        delta_t = (jtp + 1.0 - jtm - 1.0) / 2.0 * DESIRED_ANGULAR_SPEED / SAMPLING_PERIOD
        set_robot_target(x + delta_x, y + delta_y, theta + delta_t)

    for i in range(3):
        if joystick.get_button(JOYSTICK_STEPPER_BUTTONS[i]):
            stepper_pos[i] += joystick.get_axis(JOYSTICK_STEPPER_AXIS) * DESIRED_STEPPER_INCREMENT
            move_stepper(i, stepper_pos[i])
    clock.tick(SAMPLING_RATE)
