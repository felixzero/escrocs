#!/usr/bin/env python3

from typing import Tuple
import pygame
import requests
import multiprocessing
import sys
import math
import time

ROBOT_IP = "192.168.6.1"
VIRTUAL_TARGET_DISTANCE = 400.0
VIRTUAL_TARGET_ANGLE = 0.5

def get_robot_pose() -> Tuple[float, float, float]:
    time.sleep(0.2)
    pose = requests.post(f"http://{ROBOT_IP}/action/get_pose", "{}").json()
    return pose["x"], pose["y"], pose["theta"]

def send_robot_target(current_pose: Tuple[float, float, float], direction: Tuple[float, float, float]) -> None:
    time.sleep(0.2)
    xc, yc, tc = current_pose
    xd, yd, td = direction

    r = requests.post(f"http://{ROBOT_IP}/action/set_pose", json={
        "x": xc + VIRTUAL_TARGET_DISTANCE * xd,
        "y": yc + VIRTUAL_TARGET_DISTANCE * yd,
        "theta": tc + VIRTUAL_TARGET_ANGLE * td,
        "perform_detection": False
    })

PX_PER_MM = 0.33
TABLE_WIDTH = 2000
TABLE_HEIGHT = 3000

screen = pygame.display.set_mode((int(TABLE_WIDTH * PX_PER_MM), int(TABLE_HEIGHT * PX_PER_MM)))

background_image = pygame.image.load("table.png")
robot_image = pygame.image.load("robot2.png")
robot_rect = robot_image.get_rect()
robot_pose = 0.0, 0.0, 0.0

get_pose_pool = multiprocessing.Pool(1)
get_pose_result = None
set_pose_pool = multiprocessing.Pool(1)
set_pose_result = None

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            sys.exit()

    if get_pose_result is None or get_pose_result.ready():
        if get_pose_result is not None:
            robot_pose = get_pose_result.get()
        get_pose_result = get_pose_pool.apply_async(get_robot_pose)
    
    keys = pygame.key.get_pressed()
    direction_x = 1.0 if keys[pygame.K_d] else -1.0 if keys[pygame.K_q] else 0.0
    direction_y = 1.0 if keys[pygame.K_s] else -1.0 if keys[pygame.K_z] else 0.0
    direction_t = 1.0 if keys[pygame.K_a] else -1.0 if keys[pygame.K_e] else 0.0
    if (set_pose_result is None or set_pose_result.ready()) and (direction_x != 0.0 or direction_y != 0.0 or direction_t != 0.0):
        set_pose_result = set_pose_pool.apply_async(send_robot_target, (robot_pose, (direction_x, direction_y, direction_t)))

    screen.blit(background_image, (0, 0, TABLE_WIDTH * PX_PER_MM, TABLE_HEIGHT * PX_PER_MM))
    
    x, y, theta = robot_pose
    robot_rect.center = int(x * PX_PER_MM), int(y * PX_PER_MM)
    screen.blit(pygame.transform.rotate(robot_image, -theta * 180.0 / math.pi), robot_rect)
    pygame.display.flip()
