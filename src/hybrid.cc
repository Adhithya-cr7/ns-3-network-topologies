#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/netanim-module.h"
#include <cmath> 

using namespace ns3;

int main (int argc, char *argv[])
{
  // --- 1. NODE CREATION (Total 23) ---
  // Left: Star (1 Hub + 11 Spokes = 12 Nodes)
  NodeContainer starNodes;
  starNodes.Create (12);
  Ptr<Node> starHub = starNodes.Get (0);

  // Right: Bus (11 Nodes)
  NodeContainer busNodes;
  busNodes.Create (11);

  // --- 2. CONFIGURE HELPERS ---
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", StringValue ("6560ns"));

  InternetStackHelper stack;
  stack.Install (starNodes);
  stack.Install (busNodes);

  Ipv4AddressHelper address;

  // --- 3. BUILD STAR (Left Side) ---
  // Connect Hub to all Spokes
  for (uint32_t i = 1; i < 12; ++i) {
      NetDeviceContainer link = p2p.Install (starHub, starNodes.Get (i));
      std::ostringstream subnet;
      subnet << "10.1." << i << ".0";
      address.SetBase (subnet.str ().c_str (), "255.255.255.0");
      address.Assign (link);
  }

  // --- 4. BUILD BUS (Right Side) ---
  // Connect all Bus nodes to CSMA
  NetDeviceContainer busDevices = csma.Install (busNodes);
  address.SetBase ("10.2.1.0", "255.255.255.0");
  address.Assign (busDevices);

  // --- 5. BRIDGE THEM (Center) ---
  // Connect Star Hub -> Middle Bus Node (Node 5)
  NetDeviceContainer bridge = p2p.Install (starHub, busNodes.Get (5));
  address.SetBase ("10.3.1.0", "255.255.255.0");
  address.Assign (bridge);

  // --- 6. VISUALIZATION (Non-Intersecting Layout) ---
  AnimationInterface anim ("hybrid_star_bus.xml");

  // A. STAR LAYOUT (Left Zone: X=30)
  // Drawn as a Circle
  anim.SetConstantPosition (starHub, 30.0, 50.0);
  for (uint32_t i = 1; i < 12; ++i) {
      double angle = 2 * 3.14159 * i / 11;
      anim.SetConstantPosition (starNodes.Get (i), 
          30.0 + 20.0 * cos (angle), 
          50.0 + 20.0 * sin (angle));
  }

  // B. BUS LAYOUT (Right Zone: X=120)
  // Drawn as a VERTICAL Line (Top to Bottom)
  // This looks completely different from the Star circle.
  for (uint32_t i = 0; i < 11; ++i) {
      anim.SetConstantPosition (busNodes.Get (i), 
          120.0,            // Fixed X (Right side)
          20.0 + (i * 6.0)  // Varying Y (Vertical)
      );
  }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
