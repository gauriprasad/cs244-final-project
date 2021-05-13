/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2013 ResiliNets, ITTC, University of Kansas
 *
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
 * Authors: Justin P. Rohrer, Truc Anh N. Nguyen <annguyen@ittc.ku.edu>, Siddharth Gangadhar <siddharth@ittc.ku.edu>
 *
 * James P.G. Sterbenz <jpgs@ittc.ku.edu>, director
 * ResiliNets Research Group  http://wiki.ittc.ku.edu/resilinets
 * Information and Telecommunication Technology Center (ITTC)
 * and Department of Electrical Engineering and Computer Science
 * The University of Kansas Lawrence, KS USA.
 *
 * Work supported in part by NSF FIND (Future Internet Design) Program
 * under grant CNS-0626918 (Postmodern Internet Architecture),
 * NSF grant CNS-1050226 (Multilayer Network Resilience Analysis and Experimentation on GENI),
 * US Department of Defense (DoD), and ITTC at The University of Kansas.
 *
 * “TCP Westwood(+) Protocol Implementation in ns-3”
 * Siddharth Gangadhar, Trúc Anh Ngọc Nguyễn , Greeshma Umapathi, and James P.G. Sterbenz,
 * ICST SIMUTools Workshop on ns-3 (WNS3), Cannes, France, March 2013
 */

#include <iostream>
#include <fstream>
#include <string>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/error-model.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "ns3/enum.h"
#include "ns3/event-id.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/traffic-control-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Figure1213");

static bool firstCwnd = true;
static bool firstSshThr = true;
static Ptr<OutputStreamWrapper> cWndStream;
static Ptr<OutputStreamWrapper> ssThreshStream;
static uint32_t cWndValue;
static uint32_t ssThreshValue;

static void
CwndTracer (uint32_t oldval, uint32_t newval)
{
  if (firstCwnd)
  {
    *cWndStream->GetStream () << "0.0 " << oldval << std::endl;
    firstCwnd = false;
  }
  *cWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  cWndValue = newval;

  if (!firstSshThr)
  {
    *ssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << ssThreshValue << std::endl;
  }
}

static void
SsThreshTracer (uint32_t oldval, uint32_t newval)
{
  if (firstSshThr)
  {
    *ssThreshStream->GetStream () << "0.0 " << oldval << std::endl;
    firstSshThr = false;
  }
  *ssThreshStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval << std::endl;
  ssThreshValue = newval;

  if (!firstCwnd)
  {
    *cWndStream->GetStream () << Simulator::Now ().GetSeconds () << " " << cWndValue << std::endl;
  }
}

static void
TraceCwnd (std::string cwnd_tr_file_name)
{
  AsciiTraceHelper ascii;
  cWndStream = ascii.CreateFileStream (cwnd_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/CongestionWindow", MakeCallback (&CwndTracer));
}

static void
TraceSsThresh (std::string ssthresh_tr_file_name)
{
  AsciiTraceHelper ascii;
  ssThreshStream = ascii.CreateFileStream (ssthresh_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/SlowStartThreshold", MakeCallback (&SsThreshTracer));
}

// Network topology
//
//       s ----------- bs ---------- d
//           10 Mbps        2 Mbps
//            45 ms         0.01 ms
//
int main (int argc, char *argv[])
{
  // Enable logging
  LogComponentEnable("Figure1213", LOG_LEVEL_ALL);

  std::string transport_prot = "TcpWestwood";
  double error_p = 0.005;
  std::string bw_wireless = "2Mbps";
  std::string delay_wireless = "0.01ms";
  std::string bw_wired = "10Mbps";
  std::string delay_wired = "45ms";
  std::string dir = "outputs/figure1213/";
  uint32_t mtu_bytes = 400;
  double time = 100.0;
  bool sack = false;
  std::string recovery = "ns3::TcpClassicRecovery";

  CommandLine cmd (__FILE__);
  cmd.AddValue ("transport_prot", "Transport protocol to use: TcpNewReno, TcpLinuxReno, "
                                  "TcpHybla, TcpHighSpeed, TcpHtcp, TcpVegas, TcpScalable, TcpVeno, "
                                  "TcpBic, TcpYeah, TcpIllinois, TcpWestwood, TcpWestwoodPlus, TcpLedbat, "
                                  "TcpLp, TcpDctcp, TcpCubic", transport_prot);
  cmd.AddValue ("dir", "Prefix of output trace file", dir);
  cmd.Parse (argc, argv);

  if (transport_prot.compare("TcpWestwood") == 0 || transport_prot.compare("TcpWestwoodPlus") == 0)
  {
    NS_LOG_LOGIC ("Figure 12. TCP Westwood over lossy link — cwin and ssthresh");
  }
  else if (transport_prot.compare("TcpNewReno") == 0)
  {
    NS_LOG_LOGIC ("Figure 13. TCP Reno over lossy link — cwin and ssthresh");
  }
  else if (transport_prot.compare("TcpVegas") == 0)
  {
    NS_LOG_LOGIC ("Extension Figure. TCP Vegas over lossy link — cwin and ssthresh");
  }

  dir = "outputs/figure1213/" + transport_prot + "/";
  transport_prot = std::string ("ns3::") + transport_prot;

  SeedManager::SetSeed (1);
  SeedManager::SetRun (0);

  // Calculate the ADU size
  Header* temp_header = new Ipv4Header ();
  uint32_t ip_header = temp_header->GetSerializedSize ();
  temp_header = new TcpHeader ();
  uint32_t tcp_header = temp_header->GetSerializedSize ();
  uint32_t tcp_adu_size = mtu_bytes - 20 - (ip_header + tcp_header);

  /******** Set TCP defaults ********/
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

  // Create gateways, sources, and sinks
  NodeContainer gateways;
  gateways.Create (1);
  NodeContainer sources;
  sources.Create (1);
  NodeContainer sinks;
  sinks.Create (1);

  // Configure the error model
  // Here we use RateErrorModel with packet error rate
  Ptr<ExponentialRandomVariable> uv = CreateObject<ExponentialRandomVariable> ();
  uv->SetStream (50);
  RateErrorModel error_model;
  error_model.SetRandomVariable (uv);
  error_model.SetUnit (RateErrorModel::ERROR_UNIT_PACKET);
  error_model.SetRate (error_p);

  PointToPointHelper UnReLink;
  UnReLink.SetDeviceAttribute ("DataRate", StringValue (bw_wireless));
  UnReLink.SetChannelAttribute ("Delay", StringValue (delay_wireless));
  UnReLink.SetDeviceAttribute ("ReceiveErrorModel", PointerValue (&error_model));

  InternetStackHelper stack;
  stack.InstallAll ();

  TrafficControlHelper tchPfifo;
  tchPfifo.SetRootQueueDisc ("ns3::PfifoFastQueueDisc");

  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.255.255.0");

  // Configure the sources and sinks net devices
  // and the channels between the sources/sinks and the gateways
  PointToPointHelper LocalLink;
  LocalLink.SetDeviceAttribute ("DataRate", StringValue (bw_wired));
  LocalLink.SetChannelAttribute ("Delay", StringValue (delay_wired));

  Ipv4InterfaceContainer sink_interfaces;

  DataRate access_b (bw_wired);
  DataRate bottle_b (bw_wireless);
  Time access_d (delay_wired);
  Time bottle_d (delay_wireless);

  uint32_t size = static_cast<uint32_t>((std::min (access_b, bottle_b).GetBitRate () / 8) *
                                        ((access_d + bottle_d) * 2).GetSeconds ());

  Config::SetDefault ("ns3::PfifoFastQueueDisc::MaxSize",
                      QueueSizeValue (QueueSize (QueueSizeUnit::PACKETS, size / mtu_bytes)));

  NetDeviceContainer devices;
  devices = LocalLink.Install (sources.Get (0), gateways.Get (0));
  tchPfifo.Install (devices);
  address.NewNetwork ();
  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  devices = UnReLink.Install (gateways.Get (0), sinks.Get (0));

  tchPfifo.Install (devices);

  address.NewNetwork ();
  interfaces = address.Assign (devices);
  sink_interfaces.Add (interfaces.Get (1));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  uint16_t port = 50000;
  Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), port));
  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);

  AddressValue remoteAddress (InetSocketAddress (sink_interfaces.GetAddress (0, 0), port));
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (tcp_adu_size));
  BulkSendHelper ftp ("ns3::TcpSocketFactory", Address ());
  ftp.SetAttribute ("Remote", remoteAddress);
  ftp.SetAttribute ("SendSize", UintegerValue (tcp_adu_size));

  ApplicationContainer sourceApp = ftp.Install (sources.Get (0));
  sourceApp.Start (Seconds (0));
  sourceApp.Stop (Seconds (time));

  sinkHelper.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
  ApplicationContainer sinkApp = sinkHelper.Install (sinks.Get (0));
  sinkApp.Start (Seconds (0));
  sinkApp.Stop (Seconds (time));

  // Set up tracing
  Simulator::Schedule (Seconds (0.00001), &TraceCwnd, dir + "cwnd.tr");
  Simulator::Schedule (Seconds (0.00001), &TraceSsThresh, dir + "ssthresh.tr");

  // Run the simulator
  Simulator::Stop (Seconds (time));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
