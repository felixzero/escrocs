x_initial, y_initial, theta_initial = 0, 0, 0
pushback_distance = 200

function on_init(side)
    overwrite_pose(x_initial, y_initial, theta_initial)
end

function move(x, y, t)
	set_pose(x, y, t, false)
    sleep(0.2)
	while not is_motion_done() do
		sleep(0.2)
	end
end

--Careful : rotate first before using this function ! must be used only on straight line
function move_with_pushback(x, y, t)
    local first_x, first_y, first_theta = get_pose()
    local stucked_x, stucked_y, stucked_theta;
    local is_pushback = false
	set_pose(x, y, t, true)
    while true do
        sleep(0.2)
        if is_blocked() then
            stucked_x, stucked_y, stucked_theta = get_pose()
            direction_vector_x = first_x - stucked_x
            direction_vector_y = first_y - stucked_y
            norm_x = direction_vector_x / math.sqrt(direction_vector_x^2 + direction_vector_y^2)
            norm_y = direction_vector_y / math.sqrt(direction_vector_x^2 + direction_vector_y^2)
            pushback_x = stucked_x + pushback_distance * norm_x
            pushback_y = stucked_y + pushback_distance * norm_y
            set_pose(pushback_x, pushback_y, stucked_theta, true)
            is_pushback = true
        end
        if is_motion_done() then
            if is_pushback then
                is_pushback = false
                set_pose(x, y, t, true)
            end
            else
                break --reached true target
            end
        end
    end
	while not is_motion_done() do
        if is_blocked() then 
            set_pose(first_x, first_y, first_theta, true)
            sleep(0.2)
		sleep(0.2)
	end
end

function on_run()
    move(500, 0, 0)
end

function resume_loop()
    return 100
end
