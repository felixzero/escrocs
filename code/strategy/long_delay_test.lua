x_initial, y_initial, theta_initial = 0, 0, 0

function on_init()
    overwrite_pose(x_initial, y_initial, theta_initial)
end

function on_run()
    print("Ready to rock") 
    sleep(1.0)
    print("sleep long")
    sleep(80.0)
    print("beggining move")
    set_pose(-100.0, 0, 0, false)
    sleep(5.0)
    print("middle move")
    set_pose(0, 0, 0, false)
    sleep(2.0)
    print("end move")
    set_pose(-100, 0, 0, false)
    print("end of long sleep")

end

function on_end()
    print("on_end")
end
