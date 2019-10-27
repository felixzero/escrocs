#!/usr/bin/python3

import time
import itertools
import matplotlib.pyplot as plt
import matplotlib.animation as anim
import numpy as np

from serial_poll import *

def calc_raw(x):
    return x

plotter_variables = [
    #{
    #    'name'  : 'Encoder 1',
    #    'value' : (calc_raw, [1]),
    #    'graph' : 1,
    #},
    #{
    #    'name'  : 'Encoder 2',
    #    'value' : (calc_raw, [2]),
    #    'graph' : 1,
    #},
    {
        'name'  : 'Forward motion',
        'value' : (lambda e1, e2: (e1 + e2)/2.0, [1, 2]),
        'graph' : 1,
    },
    #{
    #    'name'  : 'Rotation',
    #    'value' : (lambda e1, e2: (e1 - e2)/2.0, [1, 2]),
    #    'graph' : 1,
    #},
]

if __name__ == "__main__":
    sp = SerialPoll()
    sp.start()

    plt.ion()
    fig = plt.figure()
    ax = fig.add_subplot(111)
    
    is_open = True
    while is_open:
        time.sleep(0.5)

        ax.clear()
        
        for variable in plotter_variables:
            variable_function, variable_raws = variable['value']
            
            raws = [sp.get_channel_data(i) for i in variable_raws]
            if None in raws:
                continue
            
            all_x_values = np.array(list(itertools.chain(*[[i for i, _ in r] for r in raws])))
            interpolated_raws = [np.interp(all_x_values,
                                           np.array([i for i, _ in r]),
                                           np.array([i for _, i in r])) for r in raws]
            
            calculated_value = variable_function(*interpolated_raws)
            ax.plot(all_x_values/1000.0, calculated_value, 'x', label = variable['name'])
        
        plt.xlabel('Time (s)')
        plt.legend()
        
        fig.canvas.draw()

    sp.stop()
    sp.join()
