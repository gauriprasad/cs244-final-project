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

NS_LOG_COMPONENT_DEFINE ("Figure7");

static double TRACE_START_TIME = 0.05;

static void
SeqnumTracer (Ptr<OutputStreamWrapper> stream,
              SequenceNumber32 oldval, SequenceNumber32 newval)
{
  NS_LOG_INFO (Simulator::Now ().GetSeconds () << " Sequence numbers from " << oldval << " to " << newval);

  *stream->GetStream () << Simulator::Now ().GetSeconds () << " "
                        << newval << std::endl;
}

/*** Tracers for node 0 ***/

static void
TraceSeqnum0_0 (Ptr<OutputStreamWrapper> seqnumStream)
{
  Config::ConnectWithoutContext ("/NodeList/0/$ns3::TcpL4Protocol/SocketList/0/HighestSequence",
                                 MakeBoundCallback (&SeqnumTracer, seqnumStream));
}

static void
TraceSeqnum0_1 (Ptr<OutputStreamWrapper> seqnumStream)
{
  Config::ConnectWithoutContext ("/NodeList/0/$ns3::TcpL4Protocol/SocketList/1/HighestSequence",
                                 MakeBoundCallback (&SeqnumTracer, seqnumStream));
}

static void
TraceSeqnum0_2 (Ptr<OutputStreamWrapper> seqnumStream)
{
  Config::ConnectWithoutContext ("/NodeList/0/$ns3::TcpL4Protocol/SocketList/2/HighestSequence",
                                 MakeBoundCallback (&SeqnumTracer, seqnumStream));
}

static void
TraceSeqnum0_3 (Ptr<OutputStreamWrapper> seqnumStream)
{
  Config::ConnectWithoutContext ("/NodeList/0/$ns3::TcpL4Protocol/SocketList/3/HighestSequence",
                                 MakeBoundCallback (&SeqnumTracer, seqnumStream));
}

static void
TraceSeqnum0_4 (Ptr<OutputStreamWrapper> seqnumStream)
{
  Config::ConnectWithoutContext ("/NodeList/0/$ns3::TcpL4Protocol/SocketList/4/HighestSequence",
                                 MakeBoundCallback (&SeqnumTracer, seqnumStream));
}

/*** Tracers for node 1 ***/

static void
TraceSeqnum1_0 (Ptr<OutputStreamWrapper> seqnumStream)
{
  Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/0/HighestSequence",
                                 MakeBoundCallback (&SeqnumTracer, seqnumStream));
}

static void
TraceSeqnum1_1 (Ptr<OutputStreamWrapper> seqnumStream)
{
  Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/1/HighestSequence",
                                 MakeBoundCallback (&SeqnumTracer, seqnumStream));
}

static void
TraceSeqnum1_2 (Ptr<OutputStreamWrapper> seqnumStream)
{
  Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/2/HighestSequence",
                                 MakeBoundCallback (&SeqnumTracer, seqnumStream));
}

static void
TraceSeqnum1_3 (Ptr<OutputStreamWrapper> seqnumStream)
{
  Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/3/HighestSequence",
                                 MakeBoundCallback (&SeqnumTracer, seqnumStream));
}

static void
TraceSeqnum1_4 (Ptr<OutputStreamWrapper> seqnumStream)
{
  Config::ConnectWithoutContext ("/NodeList/1/$ns3::TcpL4Protocol/SocketList/4/HighestSequence",
                                 MakeBoundCallback (&SeqnumTracer, seqnumStream));
}


void
setup_application(Ptr<Node> s, Ptr<Node> d, Ipv4InterfaceContainer rd_interfaces, int time, int receiverPort)
{
  // Receiver application
  AddressValue receiverAddress (InetSocketAddress (rd_interfaces.GetAddress (1),
                                                   receiverPort));
  PacketSinkHelper receiverHelper ("ns3::TcpSocketFactory", receiverAddress.Get());
  receiverHelper.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));

  // Install receiver application
  ApplicationContainer receiverApp = receiverHelper.Install (d);
  receiverApp.Start (Seconds (0.0));
  receiverApp.Stop (Seconds ((double)time));

  // Sender application
  BulkSendHelper ftp ("ns3::TcpSocketFactory", Address ());
  ftp.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
  ftp.SetAttribute ("Remote", receiverAddress);

  // Install sender application
  ApplicationContainer sourceApp = ftp.Install (s);
  sourceApp.Start (Seconds (0.0));
  sourceApp.Stop (Seconds ((double)time));
}

int
main (int argc, char *argv[])
{
  NS_LOG_UNCOND ("Figure 7: Average throughput vs. No. of Reno connections");
  int time = 200; // seconds
  int numReno = 0;

  CommandLine cmd (__FILE__);
  cmd.AddValue ("numReno", "Number of Reno connections (5 total)", numReno);
  cmd.Parse (argc, argv);

  /********** Declare output files **********/
  
  AsciiTraceHelper asciiTraceHelper;
  std::string dir = "outputs/figure7/";

  std::string seqnumStreamName0 = dir + std::to_string(numReno) + "reno_seqnums0.tr";
  Ptr<OutputStreamWrapper> seqnumStream0 = asciiTraceHelper.CreateFileStream (seqnumStreamName0);

  std::string seqnumStreamName1 = dir + std::to_string(numReno) + "reno_seqnums1.tr";
  Ptr<OutputStreamWrapper> seqnumStream1 = asciiTraceHelper.CreateFileStream (seqnumStreamName1);

  std::string seqnumStreamName2 = dir + std::to_string(numReno) + "reno_seqnums2.tr";
  Ptr<OutputStreamWrapper> seqnumStream2 = asciiTraceHelper.CreateFileStream (seqnumStreamName2);

  std::string seqnumStreamName3 = dir + std::to_string(numReno) + "reno_seqnums3.tr";
  Ptr<OutputStreamWrapper> seqnumStream3 = asciiTraceHelper.CreateFileStream (seqnumStreamName3);

  std::string seqnumStreamName4 = dir + std::to_string(numReno) + "reno_seqnums4.tr";
  Ptr<OutputStreamWrapper> seqnumStream4 = asciiTraceHelper.CreateFileStream (seqnumStreamName4);

  /********** Create Nodes **********/

  NS_LOG_UNCOND("Creating Nodes...");

  NodeContainer nodes;
  nodes.Create(4);

  Ptr<Node> s1 = nodes.Get(0);
  Ptr<Node> s2 = nodes.Get(1);
  Ptr<Node> r = nodes.Get(2);
  Ptr<Node> d = nodes.Get(3);
  
  /********** Create Channels **********/

  NS_LOG_UNCOND("Configuring Channels..."); 
  
  PointToPointHelper s1rLink;
  s1rLink.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
  s1rLink.SetChannelAttribute ("Delay", StringValue ("1ms"));

  PointToPointHelper s2rLink;
  s2rLink.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
  s2rLink.SetChannelAttribute ("Delay", StringValue ("1ms"));

  // Configure the error model
  // REMOVE TO TEST GOOD LINK
  Ptr<ExponentialRandomVariable> uv = CreateObject<ExponentialRandomVariable> ();
  uv->SetStream (50);
  RateErrorModel error_model;
  error_model.SetRandomVariable (uv);
  error_model.SetUnit (RateErrorModel::ERROR_UNIT_PACKET);
  error_model.SetRate (0.01);

  PointToPointHelper bottleneckLink;
  bottleneckLink.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  bottleneckLink.SetChannelAttribute ("Delay", StringValue ("49ms"));
  bottleneckLink.SetDeviceAttribute ("ReceiveErrorModel", PointerValue (&error_model));

  /********** Create NetDevices **********/

  NS_LOG_UNCOND("Creating NetDevices...");

  NetDeviceContainer s1r_NetDevices = s1rLink.Install (s1, r);
  NetDeviceContainer s2r_NetDevices = s2rLink.Install (s2, r);
  NetDeviceContainer rd_NetDevices = bottleneckLink.Install (r, d);

  /********** Set TCP defaults **********/

  NS_LOG_UNCOND("Setting TCP defaults...");
  
  Config::SetDefault ("ns3::TcpSocketBase::Sack", BooleanValue (false));
  Config::SetDefault ("ns3::TcpL4Protocol::RecoveryType", TypeIdValue (TypeId::LookupByName ("ns3::TcpClassicRecovery")));

  // Set packet size to 400 bytes
  PppHeader ppph;
  Ipv4Header ipv4h;
  TcpHeader tcph;
  uint32_t tcpSegmentSize = 400 
                            - ppph.GetSerializedSize ()
                            - ipv4h.GetSerializedSize ()
                            - tcph.GetSerializedSize ();
  Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(tcpSegmentSize));

  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpNewReno::GetTypeId()));
  Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue(TcpWestwood::WESTWOOD));
  Config::SetDefault ("ns3::TcpWestwood::FilterType", EnumValue(TcpWestwood::TUSTIN));

  /********** Install Internet Stack **********/

  NS_LOG_UNCOND("Installing Internet Stack...");
  
  InternetStackHelper stack;
  stack.InstallAll ();

  // Set IP addresses of nodes in network
  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.255.255.0");
  Ipv4InterfaceContainer s1r_interfaces = address.Assign (s1r_NetDevices);
  address.NewNetwork ();
  Ipv4InterfaceContainer s2r_interfaces = address.Assign (s2r_NetDevices);
  address.NewNetwork ();
  Ipv4InterfaceContainer rd_interfaces = address.Assign (rd_NetDevices);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  /********** Setting up the Application **********/
  
  NS_LOG_UNCOND("Setting up the Application...");
  
  // Set socket type for s0 and s1
  TypeId tid;
  std::string specificNode;
  
  tid = TypeId::LookupByName("ns3::TcpWestwood");
  std::stringstream node0Id;
  node0Id << nodes.Get (0)->GetId ();
  specificNode = "/NodeList/" + node0Id.str () + "/$ns3::TcpL4Protocol/SocketType";
  Config::Set (specificNode, TypeIdValue (tid));
  
  tid = TypeId::LookupByName("ns3::TcpNewReno");
  std::stringstream node1Id;
  node1Id << nodes.Get (1)->GetId ();
  specificNode = "/NodeList/" + node1Id.str () + "/$ns3::TcpL4Protocol/SocketType";
  Config::Set (specificNode, TypeIdValue (tid));
  
  // Create TCP Westwood channels

  int numWestwood = 5 - numReno;
  for (int i = 0; i < numWestwood; i++) {
    setup_application(s1, d, rd_interfaces, time, 5001 + i);
  }

  // Create TCP Reno channels
  
  for (int i = 0; i < numReno; i++) {
    setup_application(s2, d, rd_interfaces, time, 5001 + numWestwood + i);
  }

  // Schedule traces

  if (numReno == 0) {
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum0_0, seqnumStream0);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum0_1, seqnumStream1);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum0_2, seqnumStream2);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum0_3, seqnumStream3);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum0_4, seqnumStream4);
  } else if (numReno == 1) {
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum0_0, seqnumStream0);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum0_1, seqnumStream1);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum0_2, seqnumStream2);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum0_3, seqnumStream3);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum1_0, seqnumStream4);
  } else if (numReno == 2) {
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum0_0, seqnumStream0);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum0_1, seqnumStream1);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum0_2, seqnumStream2);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum1_0, seqnumStream3);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum1_1, seqnumStream4);
  } else if (numReno == 3) {
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum0_0, seqnumStream0);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum0_1, seqnumStream1);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum1_0, seqnumStream2);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum1_1, seqnumStream3);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum1_2, seqnumStream4);
  } else if (numReno == 4) {
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum0_0, seqnumStream0);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum1_0, seqnumStream1);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum1_1, seqnumStream2);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum1_2, seqnumStream3);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum1_3, seqnumStream4);
  } else {
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum1_0, seqnumStream0);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum1_1, seqnumStream1);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum1_2, seqnumStream2);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum1_3, seqnumStream3);
    Simulator::Schedule (Seconds (TRACE_START_TIME), &TraceSeqnum1_4, seqnumStream4);
  }

  /********** Run the Simulation **********/

  NS_LOG_UNCOND("Running the Simulation...");
  Simulator::Stop (Seconds ((double)time));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

