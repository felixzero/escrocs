config = {}

config.MATCH_DURATION_MS = 87000
config.DURATION_RETURN_STUCK_MS = 80000 --return to base if stuck if xxxx ms has elapsed
config.DEFAULT_LOOP_PERIOD_MS = 50

config.SERVO_CH_PINCE = 0

config.ENNEMY_RADIUS_MM = 200
config.SCAN_DURATION_MS = 330 -- 30ms margin

config.US_DECAY_TIME_MS = 500 -- US reading is invalid after 500ms

config.STEP_DISTANCE = 60 -- mm, increment between each "jump" at wall following
config.OVERSHOOT_MM = 100 -- distance to overshoot when sticking to wall
config.OVERSHOOT_THETA = 0.1 -- rad
config.ROBOT_CENTER_Y_BOTTOM = 200 -- distance wall/center robot when following bottom wall
config.DIST_BEF_ARM = 100 --mm , distance before placing arm on panel

config.PANEL_ARM_TIMEOUT_MS = 250

config.theta_pince_mur = -0.52359877559
config.pi = 3.14159265359

return config