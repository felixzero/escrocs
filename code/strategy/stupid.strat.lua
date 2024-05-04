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
	manual_deposit = {x=125, y=155},
	initial_position = {x=230, y=180},
	initial_solar = {x=200, y=90, theta=-pi/6},
	final_solar = {x=800, overdrive=10},
	plant_low = {x=1100, y=670, theta=0, push=120, back_x=500},
	plant_low_bis = {x=1250, y= 670, theta=0, push=120, back_x=200},
	garden_top = {x=950, ysafe=1800, y=1900, theta=-pi/2},
	plant_high = {x=650, y=1030, theta=pi, push=300},
	garden_left = {x=130, xsafe=230, ysafe=1700, y=1450, theta=pi },
	home_top_plant = {x=250, y=1800, theta=5 * pi/4},
	back_home = {x=150, y=1600, theta=pi},
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
	-- Solar panels x3
	move_to(coords.initial_position.x, coords.initial_position.y, coords.initial_solar.theta, false)
	move_to(coords.initial_solar.x, coords.initial_solar.y, coords.initial_solar.theta, false)
	move_to(coords.final_solar.x, coords.initial_solar.y - coords.final_solar.overdrive, coords.initial_solar.theta, true)
	overwrite_pose(coords.final_solar.x, coords.initial_solar.y, coords.initial_solar.theta)
	score = score + 15

	-- Grab plants
	move_servo(config.clamp_channel, config.clamp_high)
	move_servo(config.snow_channel, config.snow_high)
	move_to(coords.plant_low.x, coords.plant_low.y, coords.plant_low.theta, true)
	move_servo(config.snow_channel, config.snow_low)
	move_to(coords.plant_low.x + coords.plant_low.push, coords.plant_low.y, coords.plant_low.theta, true)
	move_to(coords.plant_low.x + 100, coords.plant_low.y, coords.plant_low.theta, true)
	move_servo(config.snow_channel, config.snow_high)
	move_stepper(config.stepper_channel, config.stepper_low, config.stepper_speed)
	sleep(3.0)
	move_to(coords.plant_low.x + coords.plant_low.push, coords.plant_low.y, coords.plant_low.theta, true)
	move_servo(config.clamp_channel, config.clamp_low)
	sleep(0.5)
	move_stepper(config.stepper_channel, config.stepper_high, config.stepper_speed)
	move_to(coords.plant_low.back_x, coords.plant_low.y, coords.plant_low.theta, true)

	-- Deposit plants
	move_to(coords.garden_top.x, coords.garden_top.ysafe, coords.garden_top.theta, true)
	move_to(coords.garden_top.x, coords.garden_top.y, coords.garden_top.theta, false)
	overwrite_pose(coords.garden_top.x, coords.garden_top.y - coords.initial_solar.y, coords.garden_top.theta, true)
	deposit_plant()

	-- Grab plants -- plant_low
	move_to(coords.plant_low_bis.x, coords.plant_low_bis.y, coords.plant_low_bis.theta, true)
	move_servo(config.snow_channel, config.snow_low)
	move_to(coords.plant_low_bis.x + coords.plant_low_bis.push, coords.plant_low_bis.y, coords.plant_high.theta, true)
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
	
	-- homing
	move_to(coords.back_home.x, coords.back_home.y, coords.back_home.theta, true)
	
	while true do sleep(1.0) end

	move_to(coords.back_home.x, coords.back_home.y, coords.back_home.theta, true)
	

	while true do end
end

function on_end()
	print("Score: " .. score)
end