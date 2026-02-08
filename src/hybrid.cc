#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/netanim-module.h"
#include <cmath> // Needed for circular layouts

using namespace ns3;

int main (int argc, char *argv[])
{
  // --- 1. DEFINE NODE COUNTS (Total = 40) ---
  uint32_t nStarSpokes = 13; // +1 Hub = 14 Star Nodes
  uint32_t nBusNodes = 13;
  uint32_t nRingNodes = 13;

  // --- 2. CREATE NODES ---
  NodeContainer starHub; starHub.Create (1);
  NodeContainer starSpokes; starSpokes.Create (nStarSpokes);
  NodeContainer busNodes; busNodes.Create (nBusNodes);
  NodeContainer ringNodes; ringNodes.Create (nRingNodes);

  // --- 3. CONFIGURE HELPERS ---
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", StringValue ("6560ns"));

  InternetStackHelper stack;
  stack.Install (starHub);
  stack.Install (starSpokes);
  stack.Install (busNodes);
  stack.Install (ringNodes);

  Ipv4AddressHelper address;

  // --- 4. BUILD STAR TOPOLOGY (Left) ---
  // Connect Hub to all Spokes
  for (uint32_t i = 0; i < nStarSpokes; ++i) {
      NetDeviceContainer link = p2p.Install (starHub.Get (0), starSpokes.Get (i));
      
      std::ostringstream subnet;
      subnet << "10.1." << (i + 1) << ".0";
      address.SetBase (subnet.str ().c_str (), "255.255.255.0");
      address.Assign (link);
  }

  // --- 5. BUILD BUS TOPOLOGY (Bottom Center) ---
  NetDeviceContainer busDevices = csma.Install (busNodes);
  address.SetBase ("10.2.1.0", "255.255.255.0");
  address.Assign (busDevices);

  // --- 6. BUILD RING TOPOLOGY (Right) ---
  // Connect i to i+1
  for (uint32_t i = 0; i < nRingNodes; ++i) {
      NetDeviceContainer link = p2p.Install (ringNodes.Get (i), ringNodes.Get ((i + 1) % nRingNodes));
      
      std::ostringstream subnet;
      subnet << "10.3." << (i + 1) << ".0";
      address.SetBase (subnet.str ().c_str (), "255.255.255.0");
      address.Assign (link);
  }

  // --- 7. BRIDGE THE TOPOLOGIES ---
  // Bridge Star (Hub) -> Bus (Node 0)
  NetDeviceContainer bridge1 = p2p.Install (starHub.Get (0), busNodes.Get (0));
  address.SetBase ("10.4.1.0", "255.255.255.0");
  address.Assign (bridge1);

  // Bridge Bus (Last Node) -> Ring (Node 0)
  NetDeviceContainer bridge2 = p2p.Install (busNodes.Get (nBusNodes - 1), ringNodes.Get (0));
  address.SetBase ("10.4.2.0", "255.255.255.0");
  address.Assign (bridge2);

  // --- 8. VISUALIZATION (THE IMPORTANT PART) ---
  AnimationInterface anim ("hybrid_40.xml");

  // A. STAR LAYOUT (Left Side)
  // Hub at (30, 40)
  anim.SetConstantPosition (starHub.Get (0), 30.0, 40.0);
  for (uint32_t i = 0; i < nStarSpokes; ++i) {
      double angle = 2 * 3.14159 * i / nStarSpokes;
      anim.SetConstantPosition (starSpokes.Get (i), 
          30.0 + 20.0 * cos (angle), // Radius 20
          40.0 + 20.0 * sin (angle));
  }

  // B. BUS LAYOUT (Bottom Line)
  // Stretches from X=10 to X=140 at Y=80
  for (uint32_t i = 0; i < nBusNodes; ++i) {
      anim.SetConstantPosition (busNodes.Get (i), 10.0 + (i * 10.0), 80.0);
  }

  // C. RING LAYOUT (Right Side)
  // Center at (120, 40)
  for (uint32_t i = 0; i < nRingNodes; ++i) {
      double angle = 2 * 3.14159 * i / nRingNodes;
      anim.SetConstantPosition (ringNodes.Get (i), 
          120.0 + 20.0 * cos (angle), // Radius 20
          40.0 + 20.0 * sin (angle));
  }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
