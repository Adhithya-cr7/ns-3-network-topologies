#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"
using namespace ns3;
int main (int argc, char *argv[])
{
  // 1. Create Nodes
  uint32_t nNodes = 20;
  NodeContainer nodes;
  nodes.Create (nNodes);
  // 2. Configure CSMA (The "Bus")
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));
  NetDeviceContainer devices = csma.Install (nodes);
  // 3. Install Internet Stack
  InternetStackHelper internet;
  internet.Install (nodes);
  // 4. Assign IP Addresses
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  address.Assign (devices);
  // 5. FIX: Manually Install Mobility and Set Positions
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);
  // Set visual positions for NetAnim (Linear layout)
  AnimationInterface anim ("bus.xml");
  for (uint32_t i = 0; i < nNodes; ++i)
    {
      anim.SetConstantPosition (nodes.Get (i), i * 5.0, 50.0);
    }

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
