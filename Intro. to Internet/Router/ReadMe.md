/******************************************************************************************************************************/
*                                                                                                                             *
*												Router Implementation                                                         *
*																															  *
/******************************************************************************************************************************/

	Name  : ABDUL MANAN
	ROLL# : 2020-10-0198

	Assignment# : 01 

/******************************************************************************************************************************/
What it does?
=> It is a router connecting two servers and and a client. The client can ping the servers/router, traceroute or wget from our   web servers. All these requests will first go to the router and router will forward them to the servers.

Steps:
 1 => Handling of ARP Request. ARP request can be send by a device to know the router's MAC Address for a particular interface.
 2 => Handling IP Packet fro me. If somebody ping the router, router will receive a ping request (ACMP) and will send ACMP Reply.

 3 => Handling IP Packet for someone else. After making sure a packet is valid, router forward the packet to destination address. If it does not have destination MAC Address, it will send an ARP Request to destination and after getting ARP Reply, it will take MAC from that request and forward the packet.

 4 => Sending ARP request. TO send an ARP Request, there is a functionality that makes sure router will send 5 ARP requests and if it gets an ARP Reply,it will cache that reply/MAC.

 5 => Proper Error Handling. The router also makes sure that no packet that it received or sent is ever corrupted, No packet is forwarded to an unknown destination. 

What I have learned?

 What is ARP Request/Reply?
 What is ICMP?
 How does a router work?
 What is the purpose of different fields in IP Header.
 How to understand code written by someone else. 

 /*****************************************************************************************************************************/