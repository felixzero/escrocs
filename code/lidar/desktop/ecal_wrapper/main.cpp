#include <ecal/ecal.h>
#include <ecal/msg/protobuf/publisher.h>
#include <ecal/msg/protobuf/subscriber.h>

#include <iostream>
#include <thread>
#include <chrono>

#include "../proto/lidar_data.pb.h"

extern "C" {
  #include "../../loca_lidar/amalgame.h"
  #include "../../loca_lidar/loca_lidar.h"
  #include "../../loca_lidar/pose_refinement.h"
}

#define LEGACY_ENAC_FORMAT 1
#define LIDAR_OFFSET  1.444 //rad

static eCAL::protobuf::CPublisher<enac::Lidar> amalg_pub;
void send_lidar_msg(eCAL::protobuf::CPublisher<enac::Lidar>& pub, amalgame_t* amalgames, uint8_t nb_amalgames) {
  enac::Lidar& msg = enac::Lidar();
  msg.set_nb_pts(nb_amalgames);
  for (int i = 0; i < nb_amalgames; i++)
  {
    msg.add_angles(((float) amalgames[i].avg_angle) / 100.0f );
    msg.add_distances(((float) amalgames[i].avg_dist) / 1000.0f);
    //msg.add_intensities((google::protobuf::uint32) lidar.intensities[i]);
  }
  amalg_pub.Send(msg);
  //Todo more properly : msg.mutable_angles()->Add(0, lidar.count);
}

void process_lidar(raw_lidar_t lidar) {
  //Generate amalgames
  amalgame_t* full_amalgames = (amalgame_t*) calloc(amalgame_finder_tuning.max_amalg_count, sizeof(amalgame_t));
  int nb_amalg = calc_amalgames(amalgame_finder_tuning, lidar, full_amalgames);
  send_lidar_msg(amalg_pub, full_amalgames, nb_amalg);

  //Convert to cartesian
  point_t* pts = (point_t*) malloc(nb_amalg * sizeof(point_t));
  uint16_t* avg_angles = (uint16_t*) malloc(nb_amalg * sizeof(uint16_t));
  uint16_t* avg_dists = (uint16_t*) malloc(nb_amalg * sizeof(uint16_t));
  for (size_t i = 0; i < nb_amalg; i++)
  {
    avg_angles[i] = full_amalgames[i].avg_angle;
    avg_dists[i] = full_amalgames[i].avg_dist;
  }
  convert_xy(pts, nb_amalg, avg_angles, avg_dists);

  //Calculate pose
  pose_t pose = refine_pose(pts, full_amalgames, nb_amalg, &pose_tuning);
  std::cout << "pose : " << pose.angle_rad << " x " << pose.pos.x << " y " << pose.pos.y << std::endl;
  std::cout << "associations : " << last_assos[0] << " " << last_assos[1] << " " << last_assos[2] << std::endl;

  free((void*) pts);

  
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
    auto start = std::chrono::high_resolution_clock::now();

  process_lidar(lidar);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

  std::cout << "Execution time: " << duration << " microsec" << std::endl;
}

void pose_cb(const enac::Position_old& pose_msg) {
  int32_t x = (int32_t) (pose_msg.x() * 1000.f);
  int32_t y = (int32_t) (pose_msg.y() * 1000.f);
  set_estimated_pose(x, y, pose_msg.theta() + LIDAR_OFFSET);
  //set_estimated_pose(pose_msg.x(), pose_msg.y(), pose_msg.theta() + LIDAR_OFFSET);
}

int main(int argc, char** argv)
{
  eCAL::Initialize(argc, argv, "loca_lidar");

  eCAL::protobuf::CSubscriber<enac::Lidar> raw_sub("lidar_data");
  eCAL::protobuf::CSubscriber<enac::Position_old> odom_sub("optitrack_pos");
  amalg_pub.Create("amalgames");

  raw_sub.AddReceiveCallback(std::bind(&raw_lidar_cb, std::placeholders::_2));
  odom_sub.AddReceiveCallback(std::bind(&pose_cb, std::placeholders::_2));
  while (eCAL::Ok())
  {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

  }


  // finalize eCAL API

  eCAL::Finalize();

}