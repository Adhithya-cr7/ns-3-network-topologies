#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;

int main (int argc, char *argv[]) {
  // 1. Create the Star Helper (1 Hub + 19 Spokes = 20 Nodes)
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  
  PointToPointStarHelper star (19, p2p); 

  // 2. Install Internet Stack
  InternetStackHelper internet;
  star.InstallStack (internet);

  // 3. Assign IP Addresses
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  star.AssignIpv4Addresses (address);

  // 4. FIX: Install Mobility Model Loop
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  
  // Install on Hub
  mobility.Install (star.GetHub ());

  // Install on Spokes (Looping manually because GetSpokeNodes() failed)
  for (uint32_t i = 0; i < 19; ++i)
  {
      mobility.Install (star.GetSpokeNode (i));
  }

  // 5. Setup Animation
  AnimationInterface anim ("star.xml"); 
  star.BoundingBox (100, 100, 400, 400); 

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
