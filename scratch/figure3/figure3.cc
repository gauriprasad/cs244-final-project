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
 */
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include "ns3/core-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/traffic-control-module.h"

#include "ns3/core-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Figure3");

static double TRACE_START_TIME = 0.05;

static void
EstimatedBWTracer (Ptr<OutputStreamWrapper> stream,
                   double oldval, double newval)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds () << " "
                        << newval << std::endl;
}

static void
TraceEstimatedBW (Ptr<OutputStreamWrapper> estimatedBWStream)
{
  Config::ConnectWithoutContext ("/NodeList/0/$ns3::TcpL4Protocol/SocketList/0/CongestionOps/$ns3::TcpWestwood/EstimatedBW",
                                 MakeBoundCallback (&EstimatedBWTracer, estimatedBWStream));
}

int
main (int argc, char *argv[])
{
  // Enable logging
  LogComponentEnable("Figure3", LOG_LEVEL_ALL);

  NS_LOG_LOGIC ("Figure 3: TCPW with concurrent UDP traffic: bandwidth estimation");
  std::string transport_prot = "TcpWestwood";
  int bw = 5; // Mbps
  int delay = 1000; // milliseconds
  int time = 300; // seconds

  CommandLine cmd (__FILE__);
  cmd.AddValue ("transport_prot", "Transport protocol to use: TcpWestwood, TcpWestwoodPlus",
                transport_prot);
  cmd.Parse (argc, argv);

  NS_LOG_LOGIC ("Running " + transport_prot);

  std::string bwStr = std::to_string(bw) + "Mbps";
  std::string delayStr = std::to_string(delay) + "ms";

  std::string dir = "outputs/figure3/" + transport_prot + "/";
  transport_prot = std::string ("ns3::") + transport_prot;
  std::string estimatedBWStreamName = dir + "estimated-bw.tr";
  Ptr<OutputStreamWrapper> estimatedBWStream;
  AsciiTraceHelper asciiTraceHelper;
  estimatedBWStream = asciiTraceHelper.CreateFileStream (estimatedBWStreamName);

  NodeContainer nodes;
  nodes.Create(2);

  Ptr<Node> h1 = nodes.Get(0);
  Ptr<Node> h2 = nodes.Get(1);

  PointToPointHelper link;
  link.SetDeviceAttribute ("DataRate", StringValue (bwStr));
  link.SetChannelAttribute ("Delay", StringValue (delayStr));
  NetDeviceContainer h1h2_NetDevices = link.Install (h1, h2);

  /******** Set TCP defaults ********/
  bool sack = false;
  std::string recovery = "ns3::TcpClassicRecovery";
  Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (1 << 21));
  Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (1 << 21));
  Config::SetDefault ("ns3::TcpSocketBase::Sack", BooleanValue (sack));
  Config::SetDefault ("ns3::TcpL4Protocol::RecoveryType",
                      TypeIdValue (TypeId::LookupByName (recovery)));
  Config::SetDefault("ns3::TcpWestwood::FilterType", EnumValue(TcpWestwood::TUSTIN));
  // Select TCP variant
  if (transport_prot.compare ("ns3::TcpWestwoodPlus") == 0)
  {
    // TcpWestwoodPlus is not an actual TypeId name; we need TcpWestwood here
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
    // the default protocol type in ns3::TcpWestwood is WESTWOOD
    Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
  }
  else
  {
    TypeId tcpTid;
    NS_ABORT_MSG_UNLESS (TypeId::LookupByNameFailSafe (transport_prot, &tcpTid), "TypeId " << transport_prot << " not found");
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (transport_prot)));
  }

  InternetStackHelper stack;
  stack.InstallAll ();

  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.255.255.0");
  Ipv4InterfaceContainer h1h2_interfaces = address.Assign (h1h2_NetDevices);
  address.NewNetwork ();

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // TCP
  uint16_t receiverPort = 5001;
  AddressValue receiverAddress (InetSocketAddress (h1h2_interfaces.GetAddress (1),
                                                   receiverPort));
  PacketSinkHelper receiverHelper ("ns3::TcpSocketFactory",
                                   receiverAddress.Get());
  receiverHelper.SetAttribute ("Protocol",
                               TypeIdValue (TcpSocketFactory::GetTypeId ()));

  ApplicationContainer receiverApp = receiverHelper.Install (h2);
  receiverApp.Start (Seconds (0.0));
  receiverApp.Stop (Seconds ((double)time));

  BulkSendHelper ftp ("ns3::TcpSocketFactory", Address ());
  ftp.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
  ftp.SetAttribute ("Remote", receiverAddress);

  ApplicationContainer sourceApp = ftp.Install (h1);
  sourceApp.Start (Seconds (0.0));
  sourceApp.Stop (Seconds ((double)time));

  Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceEstimatedBW, estimatedBWStream);

  // UDP
  uint16_t port = 10003;
  AddressValue addr (InetSocketAddress (h1h2_interfaces.GetAddress (1),
                                                   port));
  PacketSinkHelper udpReceiverHelper ("ns3::UdpSocketFactory",
                                   addr.Get());

  ApplicationContainer udpReceiverApp = udpReceiverHelper.Install (h2);
  udpReceiverApp.Start (Seconds (0.0));
  udpReceiverApp.Stop (Seconds ((double)time));

  OnOffHelper udpOnOffHelper ("ns3::UdpSocketFactory", Address ());
  udpOnOffHelper.SetConstantRate (DataRate ("1Mb/s"));
  udpOnOffHelper.SetAttribute ("Remote", addr);

  ApplicationContainer udpOnOffApp1 = udpOnOffHelper.Install (h1);
  udpOnOffApp1.Start (Seconds (25.0));
  udpOnOffApp1.Stop (Seconds (200.0));

  ApplicationContainer udpOnOffApp2 = udpOnOffHelper.Install (h1);
  udpOnOffApp2.Start (Seconds (50.0));
  udpOnOffApp2.Stop (Seconds (75.0));

  ApplicationContainer udpOnOffApp3 = udpOnOffHelper.Install (h1);
  udpOnOffApp3.Start (Seconds (125.0));
  udpOnOffApp3.Stop (Seconds (175.0));

  Simulator::Stop (Seconds ((double)time));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

