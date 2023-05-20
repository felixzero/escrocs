ROBOT_WIDTH = 32
ROBOT_DEPTH = 24

map_size_y = 3000
pi = 3.1415926535

START_X = 100
START_Y = 2900

FAN_PUMP_CHANNEL = 10
YEET_PUMP_CHANNEL_1 = 12
YEET_PUMP_CHANNEL_2 = 13

-- Functions -------------------------------------------
function overwrite_known_pose(x, y, theta)
    overwrite_pose(x, map_size_y - y, theta)
end

function move_to_position(x, y, theta)
    set_pose(x, map_size_y - y, theta, true)
    while not is_motion_done() do
        sleep(0.5)
    end
    sleep(0.5)
end

function move_to_position_no_detect(x, y, theta)
    set_pose(x, map_size_y - y, theta, false)
    while not is_motion_done() do
        sleep(0.5)
    end
    sleep(0.5)
end

-- MAIN ------------------------------------------------

overwrite_known_pose(START_X, START_Y, pi/2)
move_to_position(START_X, START_Y - 200, pi/2)
-- Throw cherries
set_pump(FAN_PUMP_CHANNEL, true)
set_pump(YEET_PUMP_CHANNEL_1, true)
set_pump(YEET_PUMP_CHANNEL_2, true)
sleep(50.0)
set_pump(FAN_PUMP_CHANNEL, false)
set_pump(YEET_PUMP_CHANNEL_1, false)
set_pump(YEET_PUMP_CHANNEL_2, false)

-- Move to next zone
move_to_position(START_X, 1450, pi/2)
sleep(15.0)
move_to_position(START_X, 3000 - 670, pi/2)
sleep(10.0)
move_to_position_no_detect(START_X, 3000 - 470, pi/2)
sleep(10.0)
