pi_half = 1.57079
x_initial, y_initial, theta_initial = 1200, 120, 1.57
score = 0

function on_init(side)
    overwrite_pose(x_initial, y_initial, theta_initial)
    move(x_initial, y_initial+100, theta_initial)
    move_servo(0, 8500)
    move_servo(1, 8000)
    sleep(1.0)
    move_stepper(2, -200, 0.15)
end

function move(x, y, t, is_avoidance)
    avoidance = is_avoidance or true
	set_pose(x, y, t, avoidance)
    sleep(0.2)
	while not is_motion_done() do
		sleep(0.2)
	end
end

function move_until_button(x,y,t, is_avoidance, channel_to_monitor, iteration_wait)
    avoidance = is_avoidance or true
    set_pose(x,y,t, avoidance)
    iteration = 0
    while get_button(107) == 1 or iteration > iteration_wait do
        iteration = iteration + 1
        sleep(0.02)
    end
    stop_motion()
end


function on_run()
    move(x_initial, y_initial, theta_initial, false)
    overwrite_pose(x_initial, y_initial, theta_initial)
    move_servo(0, 6000)
    sleep(0.2)
    move_servo(0, 4000)
    sleep(0.2)
    move_servo(0, 2000)
    move_stepper(2, 1100, 0.15) --preparation choppage planche
    sleep(1.0)
    score = score + 20
    print("score" .. score)

    -- deplacement vers premiere ensemble
    move(x_initial, 500, theta_initial, true) -- translation perpendi au mur
    move(780, 500, -1.57, true) -- translation parallele au mur pour chopper conserves
    move(780, 300, -1.57, true)
    move_servo(1, 3000) --deploiement bras
    move_stepper(2, -200, 0.15) --attrape  2 planches
    sleep(3.0)
    move_servo(1, 8000) --retractage bras
    sleep(0.1)
    move_stepper(2, 200, 0.15) -- transport planches
    sleep(0.5)
    move(780, 100, -1.57, false) -- translation zone de depose

    -- construction
    move_servo(1, 3000) --deploiement bras
    sleep(0.2)
    move_stepper(2, 950, 0.15) -- attrapage 1 planche
    sleep(2.5)
    move_servo(1, 8000) --retractage bras
    sleep(0.1)
    move(780, 220, -1.57, true) -- reculage
    move_stepper(2, 200, 0.15) -- transport planches
    sleep(2.0)
    move_servo(1, 3000) --deploiement bras
    move_stepper(2, 950, 0.15) -- position haute 1 planche
    sleep(2.0)
    move_servo(1, 8000) --retractage bras
    score = score + 8
    print(score)

    -- go en zone attente
    move(780, 500, -1.57, true) -- translation
    move(400, 1400, -1.57, true) -- atteinte zone attente, bras vers l'avant


end

function resume_loop()
    return 100
end

function on_end()
    move(400, 1600, -1.57, true) --atteinte zone arrivée
    score = score + 10
    print(score)
    move_stepper(2, -200, 0.15) -- bras position initiale

end
