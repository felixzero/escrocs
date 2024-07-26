# Generate one fixed lidar reading for testing purposes
import ecal.core.core as ecal_core
from ecal.core.publisher import ProtoPublisher

import time, sys, os
sys.path.append(os.path.join(os.path.dirname(__file__), '../../..')) # Avoids ModuleNotFoundError when finding generated folder
import generated.lidar_data_pb2 as lidar_data


lidar_tuple = ( (0.01, 0),
    (0.01, 1),
    (0.01, 2),
    (0.01, 3),
    (0.01, 4),
    (0.01, 5),
    (0.01, 6),
    (0.01, 7),
    (0.01, 8),
    (0.01, 9),
    (1.0, 10), #expected size : 0.18
    (1.0, 11), #expected center : (1, 15)
    (1.0, 12),
    (1.0, 13),
    (1.0, 14),
    (1.0, 15),
    (1.0, 16),
    (1.0, 17),
    (1.0, 18),
    (1.0, 19),
    (1.0, 20),
    (0.01, 21),
    (0.01, 22),
    (0.01, 23),
    (0.01, 24),
    (0.01, 25),
    (0.01, 26),
    (0.01, 27),
)
#init ecal publisher 
ecal_core.initialize(sys.argv, "eCAL_LD06_driver")

pub = ProtoPublisher("lidar_data", lidar_data.Lidar)
def publish_reading():
    #once the program finished to read a full circle reading from lidar, publlish it to eCAL with protobuf format
    lidar_msg = lidar_data.Lidar()
    lidar_msg.angles.extend([data[1] for data in lidar_tuple])
    lidar_msg.distances.extend([data[0] for data in lidar_tuple])
    pub.send(lidar_msg, ecal_core.getmicroseconds()[1])

if __name__ == "__main__":
    while True: 
        time.sleep(1)
        publish_reading()
    