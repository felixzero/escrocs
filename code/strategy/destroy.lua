x_initial, y_initial, theta_initial = 0, 0, 0

function on_init()
    print("on init")
    --overwrite_pose(x_initial, y_initial, theta_initial)
end

function on_run()
    print("Ready to rock") 
    sleep(1)
    print("end of sl eep")
    gbrecgeruihbger()
    --set_pose(1000, 0, 0, true)
end

function on_end()
    print("Score: 0")
end
