#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"
#include <sstream> // REQUIRED for the subnet text generation
#include <cmath>   // REQUIRED for sin/cos math

using namespace ns3;

int main (int argc, char *argv[])
{
  // 1. Create 15 Nodes
  uint32_t nNodes = 15;
  NodeContainer nodes;
  nodes.Create (nNodes);

  // 2. Setup Link Helper
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

  // 3. Install Internet Stack
  InternetStackHelper internet;
  internet.Install (nodes);

  // 4. Connect Every Node to Every Other Node
  Ipv4AddressHelper address;
  int linkCount = 1;

  for (uint32_t i = 0; i < nNodes; ++i)
    {
      for (uint32_t j = i + 1; j < nNodes; ++j)
        {
          // Install link between Node i and Node j
          NetDeviceContainer link = p2p.Install (nodes.Get (i), nodes.Get (j));

          // Create a unique subnet for this specific link (e.g., 10.1.1.0, 10.2.1.0)
          std::ostringstream subnet;
          subnet << "10." << linkCount << ".1.0";
          
          address.SetBase (subnet.str ().c_str (), "255.255.255.0");
          address.Assign (link);
          
          linkCount++;
        }
    }

  // 5. Setup Animation
  AnimationInterface anim ("mesh.xml");

  // 6. Set Node Positions (Circle) so they show up in NetAnim
  double centerX = 50.0;
  double centerY = 50.0;
  double radius = 40.0; 

  for (uint32_t i = 0; i < nNodes; ++i)
    {
      double theta = 2.0 * 3.14159 * i / nNodes; 
      double x = centerX + radius * cos (theta);
      double y = centerY + radius * sin (theta);
      
      anim.SetConstantPosition (nodes.Get (i), x, y);
    }

  // 7. Run
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
