import ecal.core.core as ecal_core
from ecal.core.publisher import ProtoPublisher
import time, sys

import proto.lidar_data_pb2 as lidar_data

if __name__ == '__main__':
    #init ecal publisher 
    ecal_core.initialize(sys.argv, "pose_pub_manual")

    pub = ProtoPublisher("odom_pose", lidar_data.Pose)

    while ecal_core.ok():
        pose = lidar_data.Pose()
        user = input("x (en m), y(en m), theta (en rad)   ").split(",")
        pose.x = int(float(user[0]) * 1000.0)
        pose.y = int(float(user[1]) * 1000.0)
        pose.theta = float(user[2])
        pub.send(pose)
        time.sleep(0.1)
    
    ecal_core.finalize()
