config = require("config")
utils = require("utils")


local state = {}
state.__index = state

state.score = 0

state.cb_finish = function(timestamp)
    print("unimplemented cb_finish !")
end
state.movement_state = 0 -- 0 = done, 1 = stopped, 2 = moving
-- scan_channels(tonumber("1111111111", 2)) -- full scan (mask of 10 bits)

state.start_action_stamp = 1


state.jump_count = 0
state.max_jump = 0
state.cb_jump = nil
state.is_jumping = false

state.x_dest = 0
state.y_dest = 0
state.theta_dest = 0


state.get_wpt_coords = function(wpt)
    return path_settings.table_coordinates[wpt].x, path_settings.table_coordinates[wpt].y
end

state.move_safe = function (x, y, theta)
    state.movement_state = 2
    state.x_dest = x
    state.y_dest = y
    state.theta_dest = theta
    set_pose(x,y,theta, true)
    
end


state.move_S1_phase = 0
state.move_S1 = function(timestamp)
    if state.move_S1_phase == 0 then
        print("TEST1")
        state.move_S1_phase = 1
        local x, y = state.get_wpt_coords("S1")
        state.move_safe(x, y, config.theta_pince_mur - 0.1 * 1) -- 60° in radians
    end

    if state.movement_state == 0 and state.move_S1_phase == 1 then
        print("TEST2")
        state.cb_finish(timestamp)
    end
end

state.is_following_S1 = 0
state.follow_S1 = function(timestamp)
    print("follow_S1")
    if state.is_following_S1 == 0 then
        print("TEST4")
        state.is_following_S1 = 1
        local x,y = get_pose()
        state.move_safe(700, y - 100,config.theta_pince_mur - 0.1 * 2 )
    end
    if state.movement_state == 0 and state.is_following_S1 == 1 then
        print("TEST5")
        local x,y = get_pose()
        overwrite_pose(x, 150, config.theta_pince_mur)
        state.score = state.score + 15
        print("Score: " .. state.score)
        state.cb_finish(timestamp)
    end

end

state.is_move_P11B_started = false
state.move_P11B = function(timestamp)
    if state.movement_state == 0 and state.is_move_P11B_started == false then
        local x, y = state.get_wpt_coords("P11B")
        state.move_safe(x, y, - config.pi / 2 + 0.17) -- Correction to allow robot to be on good angle
        move_stepper(0, 2000, 0.15)
        state.is_move_P11B_started = true
    end
    if state.movement_state == 0 and state.is_move_P11B_started == true then
        state.cb_finish(timestamp)
    end
end

state.push_P11B = function (timestamp)
    state.max_jump = 5
    state.cb_jump = function ()
        if state.movement_state == 0 and
        state.jump_count >= state.max_jump then
            move_servo(0, 10000) -- Position basse
            state.is_jumping = false
            state.cb_jump = nil
            state.cb_finish(timestamp)
        end
        if state.movement_state == 0 then
            local x,y, theta = get_pose()
            local x_dest, y_dest, theta_dest = x, y + config.STEP_DISTANCE, theta
            state.move_safe(x_dest, y_dest, theta_dest)
            state.jump_count = state.jump_count + 1
        end
    end
    state.is_jumping = true
end


state.fetch_plant = function (timestamp)

    print("state.start_action_stamp " .. tostring(state.start_action_stamp))

    if state.start_action_stamp == nil or state.start_action_stamp == 1 then
        state.start_action_stamp = timestamp
    end
    -- once servomotor is locked
    if timestamp - state.start_action_stamp > 300 then
        move_stepper(0, 0, 0.15)
    end
    if timestamp - state.start_action_stamp > 1500 then
        state.cb_finish(timestamp)
        -- stepper is in position
    end
end


state.is_push_plant = false
state.push_plant = function (timestamp)
    if not state.is_push_plant then
        local x, y, theta = get_pose()
        local x_dest, y_dest = state.get_wpt_coords("FAPT")
        state.move_safe(x_dest, y_dest, theta) --move "straight line"
        state.is_push_plant = true
    end
    if state.movement_state == 0 then
        state.cb_finish(timestamp)
    end

end

state.move_JTOP_started = 0
state.move_JTOP = function(timestamp)
    if state.move_JTOP_started == 0 then
        state.move_JTOP_started = 1
        local x, y = state.get_wpt_coords("AML")
        state.move_safe(x, y, - config.pi / 2)
    end
    if state.movement_state == 0 and state.move_JTOP_started == 1 then
        local x1 = get_pose()
        state.move_safe(x1, 2000, - config.pi / 2)
        state.move_JTOP_started = 2
    end
    if state.movement_state == 0 and state.move_JTOP_started == 2 then
        local x1 = get_pose()
        overwrite_pose(x1, 1850, -config.pi/2)
        state.cb_finish(timestamp)
    end
end

state.depose_plant_started = 0
state.depose_plant = function (timestamp)
    if state.depose_plant_started == 0 then
        move_stepper(0, 2000, 0.15)
        state.depose_plant_started = 1
    end
    if state.depose_plant_started == 1 
    and timestamp - state.start_action_stamp > 3000 then
        move_servo(config.SERVO_CH_PINCE, 2000)
        state.depose_plant_started = 2
    end

    if state.depose_plant_started == 2 and
    timestamp - state.start_action_stamp > 3500 then
        move_stepper(0, 0, 0.15)
        state.depose_plant_started = 3
    end

    if state.depose_plant_started == 3 and
    timestamp - state.start_action_stamp > 5000 then
        state.cb_finish(timestamp)
        state.score = state.score + 12
    end

end

state.is_homing_top = false
state.home_top = function (timestamp)
    print("home_top")
    if state.is_homing_top == false then
        state.is_homing_top = true
        local x, y = state.get_wpt_coords("FAPT")
        state.move_safe(x, y, - config.pi / 2)
    end
    if state.movement_state == 0 and state.is_homing_top == true then
        state.score = state.score + 10
        local x, y = get_pose()
        if not is_right then
            state.move_safe(x - 800, y, - config.pi / 2)
        end
        print(state.score)
        state.cb_finish(timestamp)
    end

end

-- action order

state.action_order = {}
state.action_order[1] = state.move_S1
state.action_order[2] = state.follow_S1
state.action_order[3] = state.push_P11B
state.action_order[4] = state.fetch_plant
state.action_order[5] = state.move_JTOP
state.action_order[6] = state.depose_plant
state.action_order[7] = state.home_top



state.cb_finish = function(timestamp)
    --print("cb_finished !")
    if state.action_order[1] ~= nil or true then
        --print("finished action ".. tostring(state.action_order[1]))
        table.remove(state.action_order, 1)
        state.start_action_stamp = timestamp

    end
end

function state.loop(timestamp)
 --Weird bug correction of nil
    if state.start_action_stamp == nil or state.start_action_stamp == 1 then
        state.start_action_stamp = timestamp
    end
    -- MOVEMENT LOOP
    if is_motion_done() then
        state.movement_state = 0
    end

    if state.is_jumping then
        state.cb_jump()
    end
    --if(state.movement_state.current == "moving" and is_blocked()) {
    --    state.movement_state:stop()
    --}

    -- ACTION LOOP
    if state.action_order[1] ~= nil then
        state.action_order[1](timestamp)
    end
    

end




return state