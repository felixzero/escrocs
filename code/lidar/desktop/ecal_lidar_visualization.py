from dataclasses import dataclass
import ecal.core.core as ecal_core
from ecal.core.subscriber import ProtoSubscriber, StringSubscriber

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Button
from matplotlib.patches import Rectangle
from math import atan2, pi
import sys, os 
sys.path.append(os.path.join(os.path.dirname(__file__), '../..')) # Avoids ModuleNotFoundError when finding generated folder
import proto.lidar_data_pb2 as lidar_data

LIDAR_OFFSET = 0 # TODO : investigate why it is needed on lidar visualization ?

class LidarCloudDisplay(): 
    """ Manages subscription tu an eCAL Topic of type "Lidar" and display the cloud point associated
    """
    def __init__(self, fig: plt.figure, topic_name: str, color: str, y_button = 0.1): #type: ignore
        """_summary_

        Args:
            fig (plt.figure): _description_
            topic_name (str): name of eCAL topic, type Lidar. (example : lidar_data)
            color (str): _description_ (example : 'y')
            y_button (float, optional): _description_. Defaults to 0.1.
        """
        self.topic_name = topic_name
        self.color = color

        # Drawing cloudpoint
        self.button_ax = fig.add_axes([0.005, y_button, 0.1, 0.1])
        self.button_ax.get_xaxis().set_visible(False)
        self.button_ax.get_yaxis().set_visible(False)

        # Drawing Button
        self.bdisplay = Button(self.button_ax, topic_name, color=color) #type: ignore
        self.bdisplay.label.set_fontsize(8) #type: ignore
        self.bdisplay.on_clicked(self.on_button_click)

        # Datas to display
        self.lidar_dist = np.array([0])
        self.lidar_theta = np.array([0])
        self.is_displaying = True

        # init ecal subscription
        self.sub_lidar = ProtoSubscriber(topic_name,lidar_data.Lidar)
        self.sub_lidar.set_callback(self.on_lidar_scan)

    def on_lidar_scan(self, topic_name, msg, time):
        self.lidar_dist = np.array(msg.distances)
        self.lidar_theta = np.array(msg.angles)

    def on_button_click(self, event): 
        self.is_displaying = not self.is_displaying

class Zoom():
    def __init__(self, fig: plt.figure, y_lim = 3.1): #type: ignore
        self.fig = fig
        self.y_lim = y_lim

        # Drawing cloudpoint
        self.button_ax = self.fig.add_axes([0.005, 0.05, 0.04, 0.05])
        self.button_ax.get_xaxis().set_visible(False)
        self.button_ax.get_yaxis().set_visible(False)
        # Drawing Zoom Button
        self.zoom = Button(self.button_ax, "zoom", color='grey') #type: ignore
        self.zoom.label.set_fontsize(8) #type: ignore
        self.zoom.on_clicked(self.on_button_click)

    def on_button_click(self, event):
        self.y_lim = 1.0 if self.y_lim == 3.1 else 3.1


plt.style.use('ggplot')
fig = plt.figure()
ax = fig.add_subplot(projection='polar') # type: ignore

class CorrespondanceDisplay():
    def __init__(self, amalgames: LidarCloudDisplay):
        self.amalgames = amalgames #access to the amalgames cloud point
        self.text = []
        self.r = []
        self.theta = []
        self.sub_lidar2table = StringSubscriber("beacons")
        self.sub_lidar2table.set_callback(self.on_lidar2table)

    def on_lidar2table(self, topic_name, msg, time):
        # prepare the data for annotation in text, r and theta
        lidar2table = eval(msg) #str to dict
        self.text = list(lidar2table.values()) # put the index of the table beacon ([0-4])
        try:
            self.r = [self.amalgames.lidar_dist[i] for i in lidar2table.keys()] # find the coordinate of the amalgame associated
            self.theta = [self.amalgames.lidar_theta[i] for i in lidar2table.keys()]
        except IndexError: # Seems to happen when no correspondance lidar & table found and when having lot of data flow (~20hz lidar)
            print("removing correspondance lidar/table")
            self.text = []
            self.r = []
            self.theta = []

    
    def display(self, ax):
        for i in range(len(self.text)):
            try:
                ax.text(np.deg2rad(self.theta[i]), self.r[i] ,self.text[i], color="r")
            except IndexError: #seem to happen only when quick lidar data (~20hz ?)
                print("display index error handler ?? TODO proper fix")
                pass
    
# on_lidar_scan


# position_filtered_scan

# Amalgames center

# calculated beacons positions



if __name__ == "__main__":
    print("visualization node")
    # Init ecal Communication
    ecal_core.initialize(sys.argv, "ecal_lidar_vizualisation")

    #Tuple of cloud points to display
    cloud_pts = (
        LidarCloudDisplay(fig, 'lidar_data', 'y', 0.1),
        LidarCloudDisplay(fig, 'lidar_filtered', 'g', 0.3),
        LidarCloudDisplay(fig, 'amalgames', 'b', 0.5),
    )
    corr_disp = CorrespondanceDisplay(cloud_pts[2])
    zoom_butt = Zoom(fig, 3.1)
    

    # Init matplotlib plot
    plt.show(block=False)
    while ecal_core.ok():
        ax.cla() # clear last cloud points
        ax.set_theta_zero_location("N") # North #type: ignore
        # https://stackoverflow.com/questions/26906510/rotate-theta-0-on-matplotlib-polar-plot
        ax.set_ylim([0.0, zoom_butt.y_lim]) # maximum is 3 m after basic filter, so no need to see further #type: ignore

        for cloud in cloud_pts: # Plot each cloud point from various data stream
            if cloud.is_displaying: #button clicked or not
                ax.scatter(
                    np.deg2rad(cloud.lidar_theta), 
                    cloud.lidar_dist, 
                    color=cloud.color
                )
                
        #display lidar2table
        corr_disp.display(ax)

        # Draw crude table at the right position and rotation
        # ------ BEGIN UGLY CODE WARNING ------ #
        try:
            for cx, cy in zip(tabx, taby):
                ct, cr = transform_mesh(cx, cy)
                ax.plot(ct, cr, color='black')
        except:
            pass
        # ------ END CODE WARNING ------ #
                            

        plt.pause(0.1)

    ecal_core.finalize()

