function move_to_position(x, y, theta)
    set_pose(x, y, theta, false)
    sleep(0.5)
    while not is_motion_done() do
        sleep(0.5)
    end
end

function raise_arm(channel)
    move_servo(channel, 3000)
end

function lower_arm(channel)
    move_servo(channel, 6500)
end

function lower_pump(channel)
    if channel == 1 then
        move_stepper(channel, 1450, 0.2)
    else
        move_stepper(channel, -1450, 0.2)
    end
end

function raise_pump(channel)
    if channel == 1 then
        move_stepper(channel, 0, 0.2)
    else
        move_stepper(channel, 0, 0.2)
    end
end

overwrite_pose(0, 0, 0)
move_to_position(600, 0, 0)
sleep(1)
move_to_position(0, 0, 0)
sleep(1)

for j = 1, 5, 1 do
    -- Dance 1
    lower_pump(1)
    raise_arm(1)
    sleep(2)

    for i = 1, 2, 1 do
        lower_arm(1)
        raise_pump(1)
        lower_pump(0)
        raise_arm(0)
        sleep(2)
        lower_arm(0)
        raise_pump(0)
        lower_pump(1)
        raise_arm(1)
        sleep(2)
    end

    lower_arm(1)
    raise_pump(1)
    sleep(2)

    -- Dance 2
    for i = 1, 2, 1 do
        lower_pump(0)
        lower_pump(1)
        sleep(2)
        raise_pump(0)
        raise_pump(1)
        sleep(2)
    end

    -- Dance 3A
    raise_arm(1)
    lower_pump(0)
    lower_pump(1)
    sleep(2)
    raise_pump(0)
    raise_pump(1)
    sleep(2)

    -- Dance 3B
    lower_arm(1)
    sleep(1)
    raise_arm(0)
    lower_pump(0)
    lower_pump(1)
    sleep(2)
    raise_pump(0)
    raise_pump(1)
    sleep(2)
    lower_arm(0)
end
