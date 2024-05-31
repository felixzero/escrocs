-- Strat : solar -> pick plant_low -> garden_top -> pick plant_low -> push plant_high -> home_top -> garden_side -> home_top

score = 0
pi = 3.1415926535
min_dist_solar = 1000.0 --mm    (minimum distance for doing 3 solar panel in middle)
distance_sensor_index_left = {4,5,6}
distance_sensor_index_right = {6,7,8}
reversed = nil
config = {
	stepper_channel = 2,
	stepper_speed = 0.15,
	stepper_high = 0,
	stepper_low = 2000,
	clamp_channel = 0,
	clamp_high = 2000,
	clamp_low = 10000,
	clamp_intermediate = {9000, 8000},
	clamp_delay = 0.5,
	snow_channel = 4,
	snow_low = 2800,
	snow_high = 5500,
}
coords = {
	manual_deposit = {x=125, y=155},
	initial_position = {x=230, y=180},
	initial_solar = {x=200, y=90, back_y=50, theta=-pi/6},
	final_solar = {x=800, overdrive=10},
	surprise_solar = {x=1800, back_y = 60, overdrive=30},
	plant_low = {x=625, y=670,  theta=-pi, push=140, back_x=500},
	plant_low_bis = {x=800, y= 670, ysafe=340, overdrive_y=50, theta=-pi, push=140, back_x=200},
	garden_top = {x=760, ysafe=1800, y=1900, theta=-pi/2},
	reset_home = {x=125,y=1910, overdrive_x=200, overdrive_y = 10, theta=-pi/2},
	plant_high = {x=650, y=1030, theta=pi, push=300},
	garden_left = {x=130, xsafe=230, ysafe=1700, y=1450, theta=pi },
	home_top_plant = {x=250, y=1800, theta=5 * pi/4},
	back_home = {x=150, y=1600, theta=pi},
}

function check_distance()
	distances = { get_us_readings() }

	if reversed then
		indexs = distance_sensor_index_right
	else
		indexs = distance_sensor_index_left
	end
	for index, value in ipairs(indexs) do
		dist = tonumber(distances[value])
		print("dist : ")
		print(dist)
		print(min_dist_solar)
		if (dist < min_dist_solar) or (dist < 10) or (dist > 65000) then
			return false
		end
	end
	return true
end

function move_to(x, y, theta, safe)
	set_pose(x, y, theta, safe)
	sleep(1.0)
	while not is_motion_done() do
		sleep(0.5)
	end
end

function timeout_move(x, y, theta, safe)
	set_pose (x,y, theta, safe)
	local time = 0.0
	sleep(1.0)
	while not is_motion_done() and time < 8.0 do
		time = time + 0.5
		sleep(0.5)
	end
	if time < 8.0 then
		score = score + 15
	end
end

function deposit_plant()
	move_stepper(config.stepper_channel, config.stepper_low, config.stepper_speed)
	sleep(3.0)
	for k, interm in pairs(config.clamp_intermediate) do
		move_servo(config.clamp_channel, interm)
		sleep(config.clamp_delay)
	end
	move_servo(config.clamp_channel, config.clamp_high)
	sleep(config.clamp_delay)
	move_stepper(config.stepper_channel, config.stepper_high, config.stepper_speed)
end

function on_init()
	overwrite_pose(coords.manual_deposit.x, coords.manual_deposit.y, 0)
	move_to(coords.manual_deposit.x, coords.initial_position.y, 0, false)
	move_to(coords.initial_position.x, coords.initial_position.y, 0, false)
	move_stepper(config.stepper_channel, config.stepper_high, config.stepper_speed)
	move_servo(config.snow_channel, config.snow_high)
	print("Ready")
end

function on_run()
	-- Solar panels x3
	move_to(coords.initial_position.x, coords.initial_position.y, coords.initial_solar.theta, false)
	move_to(coords.initial_solar.x, coords.initial_solar.y, coords.initial_solar.theta, false)
	move_to(coords.final_solar.x, coords.initial_solar.y - coords.final_solar.overdrive, coords.initial_solar.theta, true)
	overwrite_pose(coords.final_solar.x, coords.initial_solar.y, coords.initial_solar.theta)
	score = score + 15

	--if check_distance() then
	timeout_move(coords.surprise_solar.x, coords.initial_solar.y - coords.surprise_solar.overdrive, coords.initial_solar.theta, true)
	local cur_x = get_pose()
	overwrite_pose(cur_x, coords.initial_solar.y, coords.initial_solar.theta)
	--backoff from wall : 
	move_to(cur_x, coords.initial_solar.y + coords.initial_solar.back_y, coords.initial_solar.theta, true)
	--end


	-- Grab plants
	move_servo(config.clamp_channel, config.clamp_high)
	move_servo(config.snow_channel, config.snow_low)

	--clean space : 
	move_to(coords.plant_low_bis.x, coords.plant_low_bis.ysafe, coords.plant_low_bis.theta, true)
	move_to(coords.plant_low_bis.x, coords.plant_low_bis.y + coords.plant_low_bis.overdrive_y, coords.plant_low_bis.theta, true)
	move_to(coords.plant_low_bis.x, coords.plant_low_bis.y, coords.plant_low_bis.theta, true)


	move_servo(config.snow_channel, config.snow_low)
	move_to(coords.plant_low_bis.x + coords.plant_low_bis.push, coords.plant_low_bis.y, coords.plant_low_bis.theta, true)
	move_to(coords.plant_low_bis.x, coords.plant_low_bis.y,coords.plant_low_bis.theta, true)
	move_servo(config.snow_channel, config.snow_high)
	move_stepper(config.stepper_channel, config.stepper_low, config.stepper_speed)
	sleep(3.0)
	move_to(coords.plant_low_bis.x + coords.plant_low_bis.push, coords.plant_low_bis.y, coords.plant_low_bis.theta, true)
	move_servo(config.clamp_channel, config.clamp_low)
	sleep(0.5)
	move_stepper(config.stepper_channel, config.stepper_high, config.stepper_speed)

	-- Deposit plants
	move_to(coords.garden_top.x, coords.garden_top.ysafe, coords.garden_top.theta, true)
	move_to(coords.garden_top.x, coords.garden_top.y, coords.garden_top.theta, false)
	overwrite_pose(coords.garden_top.x, coords.garden_top.y - coords.initial_solar.y, coords.garden_top.theta, true)
	deposit_plant()
	score = score + 12

	-- resetting pose
	move_to(coords.reset_home.x - coords.reset_home.overdrive_x, 
	coords.reset_home.y - coords.reset_home.overdrive_y, 
	coords.reset_home.theta, true)

	move_to(coords.reset_home.x, 
	coords.reset_home.y + coords.reset_home.overdrive_y, 
	coords.reset_home.theta, false)
	overwrite_pose(coords.reset_home.x, coords.reset_home.y, coords.reset_home.theta)

	-- Grab plants -- plant_low
	move_servo(config.clamp_channel, config.clamp_high)
	move_servo(config.snow_channel, config.snow_high)
	move_to(coords.plant_low_bis.x, coords.plant_low_bis.y, coords.plant_low_bis.theta, true)
	move_servo(config.snow_channel, config.snow_low)
	move_to(coords.plant_low_bis.x + coords.plant_low_bis.push, coords.plant_low_bis.y, coords.plant_low_bis.theta, true)
	move_to(coords.plant_low_bis.x, coords.plant_low_bis.y, coords.plant_low_bis.theta, true)
	move_servo(config.snow_channel, config.snow_high)
	move_stepper(config.stepper_channel, config.stepper_low, config.stepper_speed)
	sleep(3.0)
	move_to(coords.plant_low_bis.x + coords.plant_low_bis.push, coords.plant_low_bis.y, coords.plant_low_bis.theta, true)
	move_servo(config.clamp_channel, config.clamp_low)
	sleep(0.5)
	move_stepper(config.stepper_channel, config.stepper_high, config.stepper_speed)

	--push plant_top
	move_to(coords.plant_high.x, coords.plant_high.y, coords.plant_high.theta, true) -- makes sure to finish theta turn before
	move_to(coords.plant_high.x, coords.plant_high.y + coords.plant_high.push, coords.plant_high.theta, true)
	
	move_to(coords.home_top_plant.x, coords.home_top_plant.y, coords.home_top_plant.theta, true)
	
	-- deposit plant
	move_to(coords.garden_left.x, coords.garden_left.ysafe, coords.garden_left.theta, true)
	move_to(coords.garden_left.x, coords.garden_left.y, coords.garden_left.theta, true) --push pot metal
	overwrite_pose(coords.garden_left.xsafe, coords.garden_left.y, coords.garden_left.theta, true)
	deposit_plant()
	score = score + 12
	
	-- homing
	move_to(coords.back_home.x, coords.back_home.y, coords.back_home.theta, true)
	score = score + 10
	
	while true do sleep(1.0) end

	move_to(coords.back_home.x, coords.back_home.y, coords.back_home.theta, true)
	

	while true do end
end

function resume_loop()
	return 1000
end
function on_end()
	print("Score: " .. score)
end