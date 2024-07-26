#include <ecal/ecal.h>
#include <ecal/msg/protobuf/publisher.h>
#include <ecal/msg/protobuf/subscriber.h>

#include <iostream>
#include <thread>

#include "../proto/lidar_data.pb.h"

extern "C" {
  #include "../loca_lidar/amalgame.h"
  #include "../loca_lidar/loca_lidar.h"
}

#define LEGACY_ENAC_FORMAT 1

static eCAL::protobuf::CPublisher<enac::Lidar> amalg_pub;
enac::Lidar& send_lidar_msg(eCAL::protobuf::CPublisher<enac::Lidar>& pub, raw_lidar_t& lidar) {
  enac::Lidar& msg = enac::Lidar();
  msg.set_nb_pts(lidar.count);
  for (int i = 0; i < lidar.count; i++)
  {
    msg.add_angles(((float) lidar.angles[i]) / 100.0f );
    msg.add_distances(((float) lidar.distances[i]) / 1000.0f);
    msg.add_intensities((google::protobuf::uint32) lidar.intensities[i]);
  }
  return msg; //Todo : make msg not local
  //Todo more properly : msg.mutable_angles()->Add(0, lidar.count);
}

void process_lidar(raw_lidar_t lidar) {
  raw_lidar_t* full_amalgames = (raw_lidar_t*) calloc(amalgame_finder_tuning.max_amalg_count, sizeof(raw_lidar_t));
  int nb_amalg = calc_amalgames(amalgame_finder_tuning, lidar, full_amalgames);
  //TOdo : fill -> raw_lidar_t display_amalgames; 
  //send_lidar_msg(amalg_pub, display_amalgames);

  //Publish amalgames
  //Convert to cartesian
  //Calculate transformation(=> pose)
  //Publish pose
  
}
void raw_lidar_cb(const enac::Lidar& lidar_msg)
{
  raw_lidar_t lidar;
  lidar.count = LEGACY_ENAC_FORMAT ? lidar_msg.angles_size() : (uint16_t) lidar_msg.nb_pts();
  lidar.angles = (uint16_t*) malloc(lidar.count * sizeof(uint16_t));
  lidar.distances = (uint16_t*) malloc(lidar.count * sizeof(uint16_t));
  lidar.intensities = (uint8_t*) malloc(lidar.count * sizeof(uint8_t));

  for (uint16_t i = 0; i < lidar.count; i++)
  {
    if(LEGACY_ENAC_FORMAT) {
      lidar.angles[i] = (uint16_t) floor((lidar_msg.angles(i) * 100.0f) + 0.5); // 1/100 of 1°
      lidar.distances[i] = (uint16_t) floor((lidar_msg.distances(i) * 1000.0f) + 0.5);
      lidar.intensities[i] = amalgame_finder_tuning.min_intensity;
    }
    else {
      throw "unimplemented new lidar format read";
    }
  }
  process_lidar(lidar);
  
}

int main(int argc, char** argv)
{
  eCAL::Initialize(argc, argv, "loca_lidar");

  eCAL::protobuf::CSubscriber<enac::Lidar> raw_sub("lidar_data");
  amalg_pub.Create("amalgames");

  raw_sub.AddReceiveCallback(std::bind(&raw_lidar_cb, std::placeholders::_2));
  while (eCAL::Ok())
  {

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

  }


  // finalize eCAL API

  eCAL::Finalize();

}