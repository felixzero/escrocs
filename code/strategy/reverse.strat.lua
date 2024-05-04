-- Strat : solar -> pick plant_low -> garden_top -> pick plant_low -> push plant_high -> home_top -> garden_side -> home_top

score = 0
pi = 3.1415926535

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
	manual_deposit = {x=2875, y=100},
	initial_position = {x=2770, y=1000, pushpot=350},
	initial_solar = {x=200, y=90, theta=-pi/6},
	final_solar = {x=800, overdrive=10},
	plant_low = {x=1900, y=670, theta=0, push=120, back_x=2400},
	plant_low_bis = {x=1700, y= 670, theta=0, push=120, back_x=2400},
	garden_right = {xsafe=1800, x=2900, y=612, theta=pi},
	garden_top = {x=950, ysafe=1800, y=1900, theta=-pi/2},
	back_home = {x=150, y=150, theta=-pi/6},
}

function move_to(x, y, theta, safe)
	set_pose(x, y, theta, safe)
	sleep(1.0)
	while not is_motion_done() do
		sleep(0.5)
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

	-- push pot garden_right
	move_to(coords.initial_position.x, coords.initial_position.y - coords.initial_position.pushpot, coords.initial_position.theta, true)
	overwrite_pose(coords.initial_position.x, coords.initial_position.y - coords.initial_position.pushpot, coords.initial_position.theta)
	
	--grab plants
	move_servo(config.clamp_channel, config.clamp_high)
	move_servo(config.snow_channel, config.snow_low)
	move_to(coords.plant_low.x, coords.plant_low.y, coords.plant_low.theta, true)
	move_to(coords.plant_low.x - coords.plant_low.push, coords.plant_low.y, coords.plant_low.theta, true)
	move_to(coords.plant_low.x, coords.plant_low.y, coords.plant_low.theta, true)
	move_servo(config.snow_channel, config.snow_high)
	move_stepper(config.stepper_channel, config.stepper_low, config.stepper_speed)
	sleep(3.0)

	move_to(coords.plant_low.x - coords.plant_low.push, coords.plant_low.y, coords.plant_low.theta, true)
	move_servo(config.clamp_channel, config.clamp_low)
	sleep(0.5)
	move_stepper(config.stepper_channel, config.stepper_high, config.stepper_speed)
	move_to(coords.plant_low.back_x, coords.plant_low.y, coords.plant_low.theta, true)

	-- Deposit plants
	move_to(coords.garden_right.x, coords.garden_right.ysafe, coords.garden_right.theta, true)
	move_to(coords.garden_right.x, coords.garden_right.y, coords.garden_right.theta, false)
	overwrite_pose(coords.garden_right.x, coords.garden_right.y, coords.garden_right.theta, true)
	deposit_plant()

	-- Grab plants -- plant_low_bis
	move_to(coords.plant_low_bis.x, coords.plant_low_bis.y, coords.plant_low_bis.theta, true)
	move_servo(config.snow_channel, config.snow_low)
	move_to(coords.plant_low_bis.x - coords.plant_low_bis.push, coords.plant_low_bis.y, coords.plant_high.theta, true)
	move_to(coords.plant_low_bis.x, coords.plant_low_bis.y, coords.plant_low_bis.theta, true)
	move_servo(config.snow_channel, config.snow_high)
	move_stepper(config.stepper_channel, config.stepper_low, config.stepper_speed)
	sleep(3.0)
	move_to(coords.plant_low_bis.x - coords.plant_low_bis.push, coords.plant_low_bis.y, coords.plant_low_bis.theta, true)
	move_servo(config.clamp_channel, config.clamp_low)
	sleep(0.5)
	move_stepper(config.stepper_channel, config.stepper_high, config.stepper_speed)
	
	-- deposit plant
	move_to(coords.garden_top.x, coords.garden_top.ysafe, coords.garden_top.theta, true)
	move_to(coords.garden_top.x, coords.garden_top.y, coords.garden_top.theta, true) --push pot metal
	overwrite_pose(coords.garden_top.x, coords.garden_top.ysafe, coords.garden_top.theta, true)
	deposit_plant()

	-- homing
	move_to(coords.back_home.x, coords.back_home.y, coords.back_home.theta, true)

	-- Solar panels x3
	move_to(coords.initial_position.x, coords.initial_position.y, coords.initial_solar.theta, false)
	move_to(coords.initial_solar.x, coords.initial_solar.y, coords.initial_solar.theta, false)
	move_to(coords.final_solar.x, coords.initial_solar.y - coords.final_solar.overdrive, coords.initial_solar.theta, true)
	overwrite_pose(coords.final_solar.x, coords.initial_solar.y, coords.initial_solar.theta)
	score = score + 15

	-- homing
	move_to(coords.back_home.x, coords.back_home.y, coords.back_home.theta, true)

	while true do sleep(1.0) end

	move_to(coords.back_home.x, coords.back_home.y, coords.back_home.theta, true)
	

	while true do end
end

function on_end()
	print("Score: " .. score)
end