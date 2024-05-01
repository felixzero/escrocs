path_settings = require("path_settings")

local utils = {}

-- VARIABLES --
-- PATH VARIABLES --
utils.us_channels = {us1=0, us2=0, us3=0, us4=0, us5=0, us6=0, us7=0, us8=0, us9=0, us10=0}
--TODO : VERIFY ANGLE !!!
utils.us_angles = {us1=36, us2=72, us3=108, us4=144, us5=180, us6=216, us7=252, us8=288, us9=324, us10=360}
utils.us_timestamp = {us1=0, us2=0, us3=0, us4=0, us5=0, us6=0, us7=0, us8=0, us9=0, us10=0}
utils.last_pose = {x=0, y=0, theta=0}
-- FUNCTIONS--
-- QUEUE FUNCTIONS --

Queue = {}
function Queue.new ()
    return {first = 0, last = -1}
end

function Queue.push(list, value)
    local last = list.last + 1
    list.last = last
    list[last] = value
end

function Queue.pop(list)
    local first = list.first
    if first > list.last then error("list is empty") end
    local value = list[first]
    list[first] = nil        -- to allow garbage collection
    list.first = first + 1
    return value
end

function Queue.is_empty(list)
    return list.first > list.last
end

-- LUA FUNCTIONS --
function utils.escrall(func, ...) --protected call, tailored for E.S.C.Ro.C.S
    local status, err = pcall(func, ...)
    if not status then
        print(err)
    end
end


-- POSITIONS FUNCTIONS --
function utils.get_distance(x1, y1, x2, y2)
    return math.sqrt((x2-x1)^2 + (y2-y1)^2)
end

-- AVOIDANCE FUNCTIONS --
function utils.is_point_in_circle(x, y, circle_x, circle_y, radius)
    return utils.get_distance(x, y, circle_x, circle_y) <= radius
end

-- returns the closest point from the start point(robot_pose) that isn't in the ennemy's circle
function utils.get_valid_pt(start_x, start_y, ennemy_x, ennemy_y)
    local closest_pt, closest_dist = nil, 100000

    for k,v in pairs(path_settings.table_coordinates) do
        local dist = utils.get_distance(start_x, start_y, v.x, v.y)

        if (dist < closest_dist and 
        utils.is_point_in_circle(v.x, v.y, ennemy_x, ennemy_y, config.ENNEMY_RADIUS_MM)) then
            closest_pt = k
            closest_dist = dist
        end
    end
    return closest_pt
end

-- PATHFINDING FUNCTIONS --
function utils.reach_closest_waypoint(start_x, start_y, opfor_x, opfor_y)
    --todo UNFINISHED
    -- find closest point to start_pose with help of path settings that avoid opfor

end



function utils.update_us_channels(timestamp)
    local readings = {get_us_readings()}
    for k,v in pairs(readings) do
        if utils.us_channels[k] ~= v then
            utils.us_channels[k] = v
            utils.us_timestamp[k] = timestamp
        end
    end
end

function utils.get_opfor_position(timestamp)
    utils.update_us_channels(timestamp)

    -- calculate position
    local x_robot, y_robot, theta_robot = get_pose() 
    local closest_dist = 100000
    local x_OPFOR, y_OPFOR = nil, nil

    for k,v in pairs(utils.us_channels) do
        if timestamp - utils.us_timestamp[k] < config.US_DECAY_TIME_MS then -- if scan still valid
            local angle = utils.us_angles[k]
            local x = x_robot + v * math.cos(theta_robot + angle)
            local y = y_robot + v * math.sin(theta_robot + angle)
            local dist = utils.get_distance(x_robot, y_robot, x, y)
            if dist < closest_dist then
                closest_dist = dist
                x_OPFOR, y_OPFOR = x, y
            end
        end
    end
    return x_OPFOR, y_OPFOR -- can be nil

end

--utils.pushback_target -> get ennemy angle, calculate a point opposite to him, if it's not on table, compensate

return utils