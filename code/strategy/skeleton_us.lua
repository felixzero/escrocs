MATCH_DURATION_MS = 87000
DEFAULT_LOOP_PERIOD_MS = 50

x_initial, y_initial, theta_initial = 0, 0, 0

main_loop = nil -- coroutine/thread
is_right = nil -- boolean

function on_init(side)
    is_right = side
    overwrite_pose(x_initial, y_initial, theta_initial)
end

function on_loop(timestamp)
    --here we do our shit
    --print("t " .. tostring(timestamp))
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
