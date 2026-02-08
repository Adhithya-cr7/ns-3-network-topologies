#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"
using namespace ns3;
int main (int argc, char *argv[])
{
  uint32_t nNodes = 20;
  NodeContainer nodes;
  nodes.Create (nNodes);
  // 1. Create Links (Point-to-Point)
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  InternetStackHelper internet;
  internet.Install (nodes);
  Ipv4AddressHelper address;
  // 2. Loop to connect Ring
  for (uint32_t i = 0; i < nNodes; ++i)
    {
      // Connect Node i to Node i+1 (wrapping around to 0 at the end)
      NetDeviceContainer link = p2p.Install (nodes.Get(i), nodes.Get((i+1) % nNodes));
      // Assign unique subnet to each link (10.1.1.0, 10.1.2.0, etc.)
      std::ostringstream subnet;
      subnet << "10.1." << (i + 1) << ".0";
      address.SetBase (subnet.str ().c_str (), "255.255.255.0");
      address.Assign (link);
    }
  // 3. FIX: Install Mobility
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);
  // 4. Setup Animation (Circular Layout)
  AnimationInterface anim ("ring.xml");
  double centerX = 50.0;
  double centerY = 50.0;
  double radius = 30.0;
  for (uint32_t i = 0; i < nNodes; ++i)
    {
       // Math to place dots in a circle
       double theta = 2.0 * 3.14159 * i / nNodes;
       double x = centerX + radius * cos (theta);
       double y = centerY + radius * sin (theta);
       anim.SetConstantPosition (nodes.Get (i), x, y);
    }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
