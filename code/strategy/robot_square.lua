ROBOT_WIDTH = 32
ROBOT_DEPTH = 24 -- /!\ not including forward pinch
ARM_LENGTH  = 5  -- dépassement de la fourche

-- Steppers channels
FORK_CHANNEL    = 0
REPLICA_CHANNEL = 1 --TBC

-- Fork Stepper positions
FORK_TOP    = -1150
FORK_BOTTOM =  2400
FORK_GRAB1  =   140 -- should have enough clearance both top & bottom
FORK_GRAB2  =    20 -- second step to hook fully
FORK_CRUISE =  -700
FORK_DROP   =    30 -- TBC

--START_X = 400 + 160
--START_Y = 120; // Dos collé au bord
START_X = 120
START_Y = 1440

map_size_y = 2000
pi = 3.1415926535

-- Functions -------------------------------------------
function overwrite_known_pose(x, y, theta)
    overwrite_pose(x, map_size_y - y, theta)
end

function move_to_position(x, y, theta)
    set_pose(x, map_size_y - y, theta, false)
    while not is_motion_done() do
        sleep(0.5)
    end
    sleep(0.5)
end

-- MAIN ------------------------------------------------

overwrite_known_pose(START_X, START_Y, 0)
-- Initialise stepper
move_stepper(FORK_CHANNEL, FORK_GRAB1, 0.15)

move_to_position(510, START_Y, 0)

-- Go down and face piedestal,
-- with an offset to grab with right arm
--<s> move_to_position(510, 404, -3*pi/4) -- y = 510-106</s>
move_to_position(510, 510, -3*pi/4)

-- Statue center is at (255,255).
-- Accounting for robot size we should aim for (330,330)
--[[ TO BE UNCOMMENTED IF NECESSARY
move_to_position(340, 340, -3*pi/4)
-- Elevate the arm to hook fully through the statue
move_stepper(FORK_CHANNEL, FORK_GRAB2, 0.15)
]]--
move_to_position(330, 330, -3*pi/4)

-- We should have harpooned the statue now
-- Raise stepper to cruise level
move_stepper(FORK_CHANNEL, FORK_CRUISE, 0.15)

-- Move back and center ourselves this time
move_to_position(400, 400, -3*pi/4)
move_to_position(400, 400, pi/4) -- centered wrt piedestal again
move_to_position(330, 330, pi/4)

-- Yeet the replica
sleep(0.5)
move_stepper(REPLICA_CHANNEL, 2000, 0.2)
sleep(0.5)

move_to_position(330, 330, pi/4)
-- Move to museum while avoiding start zone (TBC)
move_to_position(510,  510, pi/2)
move_to_position(510, 1600, pi/2)
move_to_position(510, 1600, pi/2)
-- face center of museum
move_to_position(225, 1800, pi/2)

move_to_position(225, 1880, pi/2)
move_stepper(FORK_CHANNEL, FORK_DROP, 0.15)

-- Wait for arm to catch the statue, then go back
sleep(2)
move_to_position(225, 1880, pi/2)

move_to_position(225, 1578, pi/2)













