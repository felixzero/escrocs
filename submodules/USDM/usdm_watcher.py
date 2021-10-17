import serial
import matplotlib.pyplot as plt
import numpy as np
import time

arduino = serial.Serial(port='COM6', baudrate=9600, timeout=.1)
loopContinue = True

def on_close(event):
    global loopContinue
    loopContinue = False
    print('End !')

fig, axes = plt.subplots()
fig.canvas.mpl_connect('close_event', on_close)

circle_10cm  = plt.Circle(( 0 , 0 ), radius = 10,  fill = False, ec='gray')
circle_20cm  = plt.Circle(( 0 , 0 ), radius = 20,  fill = False, ec='lightgray')
circle_30cm  = plt.Circle(( 0 , 0 ), radius = 30,  fill = False, ec='gray')
circle_40cm  = plt.Circle(( 0 , 0 ), radius = 40,  fill = False, ec='lightgray')
circle_50cm  = plt.Circle(( 0 , 0 ), radius = 50,  fill = False, ec='k', ls='--')
circle_60cm  = plt.Circle(( 0 , 0 ), radius = 60,  fill = False, ec='gray')
circle_70cm  = plt.Circle(( 0 , 0 ), radius = 70,  fill = False, ec='lightgray')
circle_80cm  = plt.Circle(( 0 , 0 ), radius = 80,  fill = False, ec='gray')
circle_90cm  = plt.Circle(( 0 , 0 ), radius = 90,  fill = False, ec='lightgray')
circle_100cm = plt.Circle(( 0 , 0 ), radius = 100, fill = False, ec='k')
circle_110cm = plt.Circle(( 0 , 0 ), radius = 110, fill = False, ec='gray')
circle_120cm = plt.Circle(( 0 , 0 ), radius = 120, fill = False, ec='lightgray')
circle_130cm = plt.Circle(( 0 , 0 ), radius = 130, fill = False, ec='gray')
circle_140cm = plt.Circle(( 0 , 0 ), radius = 140, fill = False, ec='lightgray')
circle_150cm = plt.Circle(( 0 , 0 ), radius = 150, fill = False, ec='k', ls='--')
circle_160cm = plt.Circle(( 0 , 0 ), radius = 160, fill = False, ec='gray')
circle_170cm = plt.Circle(( 0 , 0 ), radius = 170, fill = False, ec='lightgray')
circle_180cm = plt.Circle(( 0 , 0 ), radius = 180, fill = False, ec='gray')
circle_190cm = plt.Circle(( 0 , 0 ), radius = 190, fill = False, ec='lightgray')
circle_200cm = plt.Circle(( 0 , 0 ), radius = 200, fill = False, ec='k')
  
axes.set_aspect( 1 )
plt.gca().add_patch(circle_10cm  )
plt.gca().add_patch(circle_20cm  )
plt.gca().add_patch(circle_30cm  )
plt.gca().add_patch(circle_40cm  )
plt.gca().add_patch(circle_50cm  )
plt.gca().add_patch(circle_60cm  )
plt.gca().add_patch(circle_70cm  )
plt.gca().add_patch(circle_80cm  )
plt.gca().add_patch(circle_90cm  )
plt.gca().add_patch(circle_100cm )
plt.gca().add_patch(circle_110cm )
plt.gca().add_patch(circle_120cm )
plt.gca().add_patch(circle_130cm )
plt.gca().add_patch(circle_140cm )
plt.gca().add_patch(circle_150cm )
plt.gca().add_patch(circle_160cm )
plt.gca().add_patch(circle_170cm )
plt.gca().add_patch(circle_180cm )
plt.gca().add_patch(circle_190cm )
plt.gca().add_patch(circle_200cm )

plt.plot()
plt.title( 'Ultrasound Detection report' )

plt.ion()
plt.show()

while loopContinue:
    time.sleep(0.01)
    raw  = arduino.readline()
    data = raw.decode('utf-8')

    formattedData = data.split(";")

    for moduleData in formattedData:
        sensor = moduleData.split("> ")
        module = -2
        distance = -2
        if len(sensor) >= 2:
            module = int(sensor[0])
            distance = int(sensor[1])/10
            #print(module, " >> ", distance)

            theta = 0 + 90
            color = 'r'
            
            if module == 1:
                color='blue'
            elif module == 2:
                color='cyan'
                theta = 60 + 90
            elif module == 3:
                color='green'
                theta = 120 + 90
            elif module == 4:            
                color='yellow'
                theta = 180 + 90
            elif module == 5:
                color='orange'
                theta = 240 + 90
            elif module == 6:
                color='purple'
                theta = 300 + 90

            if distance * 10 == -1:
                color = 'r'
                distance = 0

            x = distance * np.cos(np.radians(theta))
            y = distance * np.sin(np.radians(theta))
            plt.plot(x, y, marker='x', color=color)
        #endif
    #End foreach
    plt.draw()
    plt.pause(0.001)
#end while
            
arduino.close()
plt.close()  
