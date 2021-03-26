/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Francisco Eduardo Balart Sanchez <balart40@hotmail.com 
 */
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
// include header we will use for the packets header & payload
// DEfine logging component
NS_LOG_COMPONENT_DEFINE("WifiSimpleAdhoc");


using namespace ns3;
double distance = 5; // m
static const uint32_t numNodes = 6;
static std::ofstream myfile;
static NodeContainer c;

void SetPosition(Ptr<Node> node, Vector position)
{
  Ptr<MobilityModel> mobility = node->GetObject<MobilityModel>();
  mobility->SetPosition(position);
}

int GetPosition(Ptr<Node> node)
{
  Ptr<MobilityModel> mobility = node->GetObject<MobilityModel>();
  return (int) mobility->GetPosition().x;
}

static void GenerateTraffic(Ptr<WifiNetDevice> wifinetdevice, uint32_t pktSize, uint32_t pktCount, Time pktInterval)
{
  if (pktCount > 0)
  {
    NS_LOG_UNCOND("%INFO: Sending packet! I am node " << wifinetdevice->GetNode()->GetId() << " my MAC is: " << wifinetdevice->GetAddress());
    static Mac48Address broadcast = Mac48Address("ff:ff:ff:ff:ff:ff");
    wifinetdevice->Send(Create<Packet>(pktSize), broadcast, wifinetdevice->GetNode()->GetId());
    Simulator::Schedule(pktInterval, &GenerateTraffic, wifinetdevice, pktSize, pktCount - 1, pktInterval);
  }
  else
  {
  }
}

void ReceivePacketWithRss(std::string context, Ptr<const Packet> packet, uint16_t channelFreqMhz, WifiTxVector txVector, MpduInfo aMpdu, SignalNoiseDbm signalNoise, uint16_t staId)
{
  WifiMacHeader hdr;
  packet->PeekHeader(hdr);

  NS_LOG_UNCOND("*******************************************************************************************************************");
  NS_LOG_UNCOND("%INFO: I am Node " << context.substr(10, 1) << " And I Recieved " << signalNoise.signal << " dbm");
  NS_LOG_UNCOND("*******************************************************************************************************************");
  if(context.substr(10, 1)=="1"){
  myfile << GetPosition(c.Get(0))<<","<<signalNoise.signal << "\n";
  }
}

bool ReceivePacket(Ptr<NetDevice> netdevice, Ptr<const Packet> packet, uint16_t protocol, const Address &sourceAddress)
{
  // Ptr<WifiNetDevice> wifinetdevice = DynamicCast<WifiNetDevice> (netdevice);
  // uint32_t pktSize = 1000;
  //NS_LOG_UNCOND ("%INFO: Received one packet! I am node "<<wifinetdevice->GetNode ()->GetId ()<<" my MAC is: "<<wifinetdevice->GetAddress());
  // NS_LOG_UNCOND ("%INFO: Received a packet from MAC:" << sourceAddress);
  //Ptr<NetDevice> devicesource = sourceAddress->GetObject<NetDevice>();
  //Ptr<Mac48Address> sourceMacAddress = GetObject<sourceAddress>;
  //Ptr<m_address> mAddressfromMacSrc = sourceMacAddress->GetObject<m_address>();
  //NS_LOG_UNCOND ("%INFO: received a packet from Node ID:" <<mAddressfromMacSrc->GetObject<Node>()->GetId());
  // *** PROTOCOL NUMBER IS MAPPED TO A SPECIFIC L3 PAYLOAD FORMAT SEE LINK BELOW
  //http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
  //NS_LOG_UNCOND ("%INFO: sending packet response due to callback with protocol: " << protocol);
  //double r =1.0+ ((double) rand() / (RAND_MAX));
  //Simulator::Schedule (Seconds (r), &GenerateTraffic, wifinetdevice, pktSize,1, Seconds(r));
  return true;
}
void installMobility(NodeContainer &c)
{
  MobilityHelper mobility;
  mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                "MinX", DoubleValue(0),
                                "MinY", DoubleValue(100),
                                "DeltaX", DoubleValue(distance),
                                "DeltaY", DoubleValue(distance),
                                "GridWidth", UintegerValue(6),
                                "LayoutType", StringValue("RowFirst"));
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(c);
  std::string traceFile = "scratch/move.ns_movements";
  Ns2MobilityHelper ns2 = Ns2MobilityHelper(traceFile);
  ns2.Install();
  return;
}

int main(int argc, char *argv[])
{
  uint32_t packetSize = 1000; // bytes
  uint32_t packets = 500;     // number
  //uint32_t numPackets = 1;
  double interval = 0.1; // seconds
                Time interPacketInterval = Seconds(interval);         // double rss = -80;  // -dBm
  myfile.open ("capture.csv", std::ofstream::out | std::ofstream::trunc);
  CommandLine cmd;
  cmd.Parse(argc, argv);

  // Convert to time object


  // Enable verbosity for debug which includes
  // NS_LOG_DEBUG_, NS_LOG_WARN and LOG_ERROR
  LogComponentEnable("WifiSimpleAdhoc", LOG_LEVEL_INFO);

  // Message to terminal console for debug
  NS_LOG_UNCOND("%INFO: Starting Test now...");

  // Create complete empty "hulls" nodes
  NS_LOG_UNCOND("%INFO: Creating Nodes...");
  // creating nodes
  c.Create(numNodes);

  NS_LOG_UNCOND("%INFO: Configuring PHY Loss model and connecting to PHY...");
  // Create PHY and Channel
  YansWifiPhyHelper wifiPhy;
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
  wifiPhy.SetChannel(wifiChannel.Create());

  NS_LOG_UNCOND("%INFO: Configuring PHY Loss model...");
  //Config::SetDefault("ns3::RangePropagationLossModel::MaxRange", DoubleValue(10));
  wifiChannel.AddPropagationLoss("ns3::LogDistancePropagationLossModel");
  //wifiChannel.SetPropagationDelay("ns3::LogDistancePropagationLossModel"); 

  // Connect PHY with the Channel
  NS_LOG_UNCOND("%INFO: Connecting PHY with Channel...");

  NS_LOG_UNCOND("%INFO: Configuring PHY STD and RSM...");
  // Create WifiHelper to be able to setup the PHY
  WifiHelper wifi; // = WifiHelper::Default ();
  wifi.SetStandard(WIFI_STANDARD_80211b);
  //wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode","OfdmRate12Mbps","ControlMode","OfdmRate12Mbps");
  // Later test: ns3::IdealWifiManager
  wifi.SetRemoteStationManager("ns3::ArfWifiManager");
  // Control the Rate via Remote Station Manager

  NS_LOG_UNCOND("%INFO: Configuring MAC...");
  // MAC layer configuration
  WifiMacHelper wifiMac;
  // Setting the type with Adhoc we will wrap and inherit the RegularMac and WifiMac classes properties
  wifiMac.SetType("ns3::AdhocWifiMac", "QosSupported", BooleanValue(false));

  // Make a device nodes with phy, mac and nodes already configured
  // Below will create WifiNetDevices ***********************************************
  NetDeviceContainer devices = wifi.Install(wifiPhy, wifiMac, c);

  NS_LOG_UNCOND("%INFO: configuring mobility.");
  // Configure mobility
  installMobility(c);

  NS_LOG_UNCOND("%INFO: Assign Mac48Address Addresses.");
  //devices->SetAddress(Mac48Address::Allocate ());
  uint32_t nDevices = devices.GetN();
  for (uint32_t i = 0; i < nDevices; ++i)
  {
    Ptr<WifiNetDevice> p = DynamicCast<WifiNetDevice>(devices.Get(i));
    p->SetAddress(Mac48Address::Allocate());
    devices.Get(i)->SetReceiveCallback(MakeCallback(&ReceivePacket));
    // p->GetPhy()->TraceConnectWithoutContext ("MonitorSnifferRx", MakeCallback (&ReceivePacketWithRss));
  }

  Config::Connect("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::WifiPhy/MonitorSnifferRx", MakeCallback(&ReceivePacketWithRss));

  Ptr<WifiNetDevice> wifinetdeviceA = DynamicCast<WifiNetDevice>(devices.Get(0));

  NS_LOG_UNCOND("%INFO: Generate traffic.");

  Simulator::ScheduleWithContext(wifinetdeviceA->GetNode()->GetId(), Seconds(1), &GenerateTraffic, wifinetdeviceA, packetSize, packets, interPacketInterval);

  // enable packet capture tracing and xml
  wifiPhy.EnablePcap("WifiSimpleAdhoc", devices);
  AnimationInterface anim("WifiSimpleAdhoc.xml");

  //Config::Connect ("/NodeList/0/DeviceList/*/$ns3::WifiNetDevice/Phy/MonitorSnifferRx", MakeCallback (&ReceivePacketWithRss));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
} //END of Main function