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

NS_LOG_COMPONENT_DEFINE ("SeqNum");

static double TRACE_START_TIME = 0.05;

static void
SeqnumFastTracer (Ptr<OutputStreamWrapper> stream,
                  SequenceNumber32 oldval, SequenceNumber32 newval)
{
  NS_LOG_INFO (Simulator::Now ().GetSeconds () <<
                                               " Sequence numbers from " << oldval << " to " << newval);

  *stream->GetStream () << Simulator::Now ().GetSeconds () << " "
                        << newval << std::endl;
}

static void
TraceSeqnumFast (Ptr<OutputStreamWrapper> seqnumFastStream)
{
  Config::ConnectWithoutContext ("/NodeList/0/$ns3::TcpL4Protocol/SocketList/0/HighestSequence",
                                 MakeBoundCallback (&SeqnumFastTracer, seqnumFastStream));
}

static void
SeqnumSlowTracer (Ptr<OutputStreamWrapper> stream,
                  SequenceNumber32 oldval, SequenceNumber32 newval)
{
  NS_LOG_INFO (Simulator::Now ().GetSeconds () <<
                                               " Sequence numbers from " << oldval << " to " << newval);

  *stream->GetStream () << Simulator::Now ().GetSeconds () << " "
                        << newval << std::endl;
}

static void
TraceSeqnumSlow (Ptr<OutputStreamWrapper> seqnumSlowStream)
{
  Config::ConnectWithoutContext ("/NodeList/0/$ns3::TcpL4Protocol/SocketList/1/HighestSequence",
                                 MakeBoundCallback (&SeqnumSlowTracer, seqnumSlowStream));
}

int
main (int argc, char *argv[])
{
  NS_LOG_UNCOND ("Figure 4: TCPW sequence numbers with 50 ms and 200 ms RTT without RED: ");
  int time = 200; // seconds
  std::string transport_prot = "TcpWestwood";

  CommandLine cmd (__FILE__);
  cmd.AddValue ("transport_prot", "Transport protocol to use: TcpWestwood, TcpReno", transport_prot);
  cmd.Parse (argc, argv);

  /********** Declare output files **********/
  
  AsciiTraceHelper asciiTraceHelper;
  std::string dir = "outputs/";

  std::string seqnumFastStreamName = dir + transport_prot + "SeqnumFast.tr";
  Ptr<OutputStreamWrapper> seqnumFastStream;
  seqnumFastStream = asciiTraceHelper.CreateFileStream (seqnumFastStreamName);
  
  std::string seqnumSlowStreamName = dir + transport_prot + "SeqnumSlow.tr";
  Ptr<OutputStreamWrapper> seqnumSlowStream;
  seqnumSlowStream = asciiTraceHelper.CreateFileStream (seqnumSlowStreamName);

  /********** Create Nodes **********/

  NS_LOG_UNCOND("Creating Nodes...");

  NodeContainer nodes;
  nodes.Create(3);

  Ptr<Node> h1 = nodes.Get(0);
  Ptr<Node> h2 = nodes.Get(1);
  Ptr<Node> h3 = nodes.Get(2);
  
  /********** Create Channels **********/

  NS_LOG_UNCOND("Configuring Channels..."); 

  PointToPointHelper bottleneckLink;
  bottleneckLink.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  bottleneckLink.SetChannelAttribute ("Delay", StringValue ("25ms"));
  //bottleneckLink.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("1p"));

  PointToPointHelper longLink;
  longLink.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
  longLink.SetChannelAttribute ("Delay", StringValue ("75ms"));
  //bottleneckLink.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("1p"));

  /********** Create NetDevices **********/

  NS_LOG_UNCOND("Creating NetDevices...");

  NetDeviceContainer h1h2_NetDevices = bottleneckLink.Install (h1, h2);
  NetDeviceContainer h2h3_NetDevices = longLink.Install (h2, h3);

  /********** Set TCP defaults **********/

  // TODO set packet size to 400 bytes? 

  /*
  PppHeader ppph;
  Ipv4Header ipv4h;
  TcpHeader tcph;
  uint32_t tcpSegmentSize = 400 
                            - ppph.GetSerializedSize ()
                            - ipv4h.GetSerializedSize ()
                            - tcph.GetSerializedSize ();
  Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(tcpSegmentSize));
  */

  // Set RED on or off

  // TODO 

  // Choose Westwood or Reno

  if (transport_prot == "TcpWestwood") {
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId()));
      Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue(TcpWestwood::WESTWOODPLUS));
      Config::SetDefault ("ns3::TcpWestwood::FilterType", EnumValue(TcpWestwood::TUSTIN));
  } else if (transport_prot == "TcpReno") {
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpNewReno::GetTypeId()));
  } else {
      NS_LOG_UNCOND("BAD TCP PROT");
  }
  
  /********** Install Internet Stack **********/

  NS_LOG_UNCOND("Installing Internet Stack...");

  InternetStackHelper stack;
  stack.InstallAll ();

  // Set IP addresses of nodes in network
  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.255.255.0");
  Ipv4InterfaceContainer h1h2_interfaces = address.Assign (h1h2_NetDevices);
  address.NewNetwork ();
  Ipv4InterfaceContainer h2h3_interfaces = address.Assign (h2h3_NetDevices);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  /********** Setting up the Application **********/
  
  NS_LOG_UNCOND("Setting up the Application...");

  /*** Fast TCP ***/

  // Receiver application
  uint16_t receiverPort = 5001;
  AddressValue receiverAddressFast (InetSocketAddress (h1h2_interfaces.GetAddress (1),
                                                       receiverPort));
  PacketSinkHelper receiverHelperFast ("ns3::TcpSocketFactory",
                                       receiverAddressFast.Get());
  receiverHelperFast.SetAttribute ("Protocol",
                                   TypeIdValue (TcpSocketFactory::GetTypeId ()));

  // Install receiver application
  ApplicationContainer receiverAppFast = receiverHelperFast.Install (h2);
  receiverAppFast.Start (Seconds (0.0));
  receiverAppFast.Stop (Seconds ((double)time));

  // Sender application
  BulkSendHelper ftpFast ("ns3::TcpSocketFactory", Address ());
  ftpFast.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
  ftpFast.SetAttribute ("Remote", receiverAddressFast);

  // Install sender application
  ApplicationContainer sourceAppFast = ftpFast.Install (h1);
  sourceAppFast.Start (Seconds (0.0));
  sourceAppFast.Stop (Seconds ((double)time));

  // Start tracing 
  Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnumFast, seqnumFastStream);

  
  /*** Slow TCP ***/

  // Receiver application
  AddressValue receiverAddressSlow (InetSocketAddress (h2h3_interfaces.GetAddress (1),
                                                   receiverPort));
  PacketSinkHelper receiverHelperSlow ("ns3::TcpSocketFactory",
                                   receiverAddressSlow.Get());
  receiverHelperSlow.SetAttribute ("Protocol",
                               TypeIdValue (TcpSocketFactory::GetTypeId ()));

  // Install receiver application
  ApplicationContainer receiverAppSlow = receiverHelperSlow.Install (h3);
  receiverAppSlow.Start (Seconds (0.0));
  receiverAppSlow.Stop (Seconds ((double)time));

  // Sender application
  BulkSendHelper ftpSlow ("ns3::TcpSocketFactory", Address ());
  ftpSlow.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
  ftpSlow.SetAttribute ("Remote", receiverAddressSlow);

  // Install sender application
  ApplicationContainer sourceAppSlow = ftpSlow.Install (h1);
  sourceAppSlow.Start (Seconds (0.0));
  sourceAppSlow.Stop (Seconds ((double)time));

  // Start tracing 
  Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnumSlow, seqnumSlowStream);

  /********** Run the Simulation **********/

  NS_LOG_UNCOND("Running the Simulation...");
  Simulator::Stop (Seconds ((double)time));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

