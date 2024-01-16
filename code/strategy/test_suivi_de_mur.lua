MATCH_DURATION_MS = 87000
DEFAULT_LOOP_PERIOD_MS = 50

x_initial, y_initial, theta_initial = 0, 0, 0

main_loop = nil -- coroutine/thread
is_left = nil -- boolean
state = 0 
-- 0 : turn to 'side', 
-- 1 : stick to wall, 
-- 2 : advance target 1
-- 3 : advance target 2
-- 4 : advance target 3
-- 5 : advance target 4
is_moving = false

function on_init(side)
    print("teeeest")
    is_left = side
    overwrite_pose(x_initial, y_initial, theta_initial)
end

function on_loop(timestamp)
    --here we do our shit
    x, y, theta = get_pose()
    print("state " .. tostring(state))

    if state == 0 then
        if is_moving == false then
            set_pose(0.0, 0.0, 1.6, false)
            is_moving = true
            return nil -- skip the rest to allow robot to move
        elseif is_motion_done() == true then
            state = 1
            is_moving = false
        end

    elseif state == 1 then
        if is_moving == false then
            set_pose(0.0, -100.0, 1.6, false)
            is_moving = true
            return nil -- skip the rest to allow robot to move
        elseif is_motion_done() == true then
            state = 2
            is_moving = false
        end

    elseif state == 2 then
        if is_moving == false then
            set_pose(200.0, -110.0, 1.6, false)
            is_moving = true
            return nil
        elseif x > 150.0 then
            overwrite_pose(x, -100.0, theta)
            set_pose(400.0, -110.0, 1.6, false)
            state = 3 
        end

    elseif state == 3 then
        if x > 350.0 then
            overwrite_pose(x, -100.0, theta)
            set_pose(600.0, -110.0, 1.6, false)
            state = 4
        end

    elseif state == 4 then
        if x > 550.0 then
            overwrite_pose(x, -100.0, theta)
            set_pose(700.0, -110.0, 1.6, false)
            state = 5
        end

    end

    return DEFAULT_LOOP_PERIOD_MS
end

function create_coroutine()
    main_loop = coroutine.create( function (timestamp)
        local start_time = timestamp
        while true do
            if (timestamp - start_time) > MATCH_DURATION_MS then
				on_end()
                break
            end
            sleep_period = on_loop(timestamp)
            timestamp = coroutine.yield(sleep_period)
        end
    end)
end


-- resume loop will receive the "start_time timestamp on first call"
function resume_loop(timestamp)
    if main_loop == nil then
        create_coroutine()
    end
    status, value = coroutine.resume(main_loop, timestamp)
    if coroutine.status (main_loop) == "dead" then
		main_loop = nil
		return -1
    end
	return value

end

function on_end()
    print("Score: 0")
end
