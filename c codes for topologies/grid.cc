#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/netanim-module.h"
using namespace ns3;
int main (int argc, char *argv[])
{
  // 1. Setup the Grid Helper (5x4)
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  // This automatically connects the nodes in a grid pattern for you
  PointToPointGridHelper grid (5, 4, p2p); 
  // 2. Install Internet Stack
  InternetStackHelper internet;
  grid.InstallStack (internet);
  // 3. Assign IP Addresses
  grid.AssignIpv4Addresses (Ipv4AddressHelper ("10.1.1.0", "255.255.255.0"),
                            Ipv4AddressHelper ("10.2.1.0", "255.255.255.0"));
  // 4. Setup Animation
  AnimationInterface anim ("grid.xml");
  for (uint32_t r = 0; r < 5; ++r)
    {
      for (uint32_t c = 0; c < 4; ++c)
        {
          // Get the node at this specific grid spot
          Ptr<Node> node = grid.GetNode (r, c);
          // Set its position: X = column * spacing, Y = row * spacing
          anim.SetConstantPosition (node, c * 20.0, r * 20.0);
        }
    }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
