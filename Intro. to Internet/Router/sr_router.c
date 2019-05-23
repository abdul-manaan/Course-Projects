/**********************************************************************
 * file:  sr_router.c
 * date:  Mon Feb 18 12:50:42 PST 2002
 * Contact: casado@stanford.edu
 *
 * Description:
 *
 * This file contains all the functions that interact directly
 * with the routing table, as well as the main entry method
 * for routing.
 *
 **********************************************************************/
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <assert.h>


#include "sr_if.h"
#include "sr_rt.h"
#include "sr_router.h"
#include "sr_protocol.h"
#include "sr_arpcache.h"
#include "sr_utils.h"
#include "vnscommand.h"
/*---------------------------------------------------------------------
 * Method: sr_init(void)
 * Scope:  Global
 *
 * Initialize the routing subsystem
 *
 *---------------------------------------------------------------------*/

void sr_init(struct sr_instance* sr)
{
    /* REQUIRES */
    assert(sr);

    /* Initialize cache and cache cleanup thread */
    sr_arpcache_init(&(sr->cache));

    pthread_attr_init(&(sr->attr));
    pthread_attr_setdetachstate(&(sr->attr), PTHREAD_CREATE_JOINABLE);
    pthread_attr_setscope(&(sr->attr), PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setscope(&(sr->attr), PTHREAD_SCOPE_SYSTEM);
    pthread_t thread;

    pthread_create(&thread, &(sr->attr), sr_arpcache_timeout, sr);
    
    /* Add initialization code here! */

} /* -- sr_init -- */

/*---------------------------------------------------------------------
 * Method: sr_handlepacket(uint8_t* p,char* interface)
 * Scope:  Global
 *
 * This method is called each time the router receives a packet on the
 * interface.  The packet buffer, the packet length and the receiving
 * interface are passed in as parameters. The packet is complete with
 * ethernet headers.
 *
 * Note: Both the packet buffer and the character's memory are handled
 * by sr_vns_comm.c that means do NOT delete either.  Make a copy of the
 * packet instead if you intend to keep it around beyond the scope of
 * the method call.
 *
 *---------------------------------------------------------------------*/



void handleArpReply(struct sr_instance* sr, uint8_t* packet, unsigned int len, char* interface) {

  sr_ethernet_hdr_t* etherHead = (sr_ethernet_hdr_t*)(packet);
  sr_arp_hdr_t* arpHeader        = (sr_arp_hdr_t*) (packet+sizeof(sr_ethernet_hdr_t));

  struct sr_arpreq* allRequests = sr_arpcache_insert(&sr->cache, arpHeader->ar_sha, arpHeader->ar_sip);
  if(!allRequests){
    return;
  }
  struct sr_packet* eachPacket = allRequests->packets;
  while(eachPacket){
      struct sr_packet* nextPacket = eachPacket->next;
      sr_ethernet_hdr_t* etherHeader = (sr_ethernet_hdr_t*)(eachPacket->buf);
      sr_ip_hdr_t*       ipHeader    = (sr_ip_hdr_t*)(eachPacket->buf +sizeof(sr_ethernet_hdr_t));

      ipHeader->ip_ttl = ipHeader->ip_ttl- 1;

      ipHeader->ip_sum = 0;
      uint16_t ipSum = cksum(ipHeader, sizeof(sr_ip_hdr_t));
      ipHeader->ip_sum = ipSum;

      struct sr_rt* nextHOPinterface = getRoutingTableEntry(sr,ipHeader->ip_dst);

      struct sr_if* outgoingInterface = sr_get_interface(sr,nextHOPinterface->interface);

      memcpy(etherHeader->ether_dhost, arpHeader->ar_sha, ETHER_ADDR_LEN);
      memcpy(etherHeader->ether_shost, outgoingInterface->addr, ETHER_ADDR_LEN);
/*      print_hdrs(eachPacket->buf, eachPacket->len);
*/      
      printf("Modified IP Header: \n");
      print_hdrs(eachPacket->buf,eachPacket->len);
      sr_send_packet(sr, eachPacket->buf,eachPacket->len,nextHOPinterface->interface );
      eachPacket = nextPacket;
  }
    sr_arpreq_destroy(&sr->cache ,allRequests);
/*    printf("Packet Forwarded after getting ARP REply\n");
*/}


void send_ip_next_hop(struct sr_instance* sr, uint8_t* packet, unsigned int len, char* interface) {

  uint8_t* packetCopy = malloc(len);
  memcpy(packetCopy,packet,len);

  sr_ethernet_hdr_t* etherHeader = (sr_ethernet_hdr_t*)(packet);
  sr_ip_hdr_t*       ipHeader    = (sr_ip_hdr_t*)(packet+sizeof(sr_ethernet_hdr_t));

  ipHeader->ip_ttl = ipHeader->ip_ttl- 1;

  ipHeader->ip_sum = 0;
  uint16_t ipSum = cksum(ipHeader, sizeof(sr_ip_hdr_t));
  ipHeader->ip_sum = ipSum;
  struct sr_rt* nextHOPinterface = getRoutingTableEntry(sr,ipHeader->ip_dst);

  if(!nextHOPinterface){
/*    printf("No Routing Entry..\n");
*/    return;
  }
  struct sr_arpentry* nextHopMac = sr_arpcache_lookup(&sr->cache, ipHeader->ip_dst);
  if(nextHopMac){
    struct sr_if* outgoingInterface = sr_get_interface(sr,nextHOPinterface->interface);
    /*Cache Hit*/
    if(!outgoingInterface){
/*      printf("No Outgoing Interface..\n");
*/    }
    memcpy(etherHeader->ether_dhost, nextHopMac->mac, ETHER_ADDR_LEN);
    memcpy(etherHeader->ether_shost, outgoingInterface->addr, ETHER_ADDR_LEN);
    printf("Modified IP Header: \n");
     print_hdrs(packet,len);

    sr_send_packet(sr,packet,len,outgoingInterface->name);
/*    sshdrs(packet,len);
    printf("Cache Hit....\n");    fflush(stdout);
*/  } else {
    /*Cache Miss

    /*Queue the ARP Request*/
    struct sr_if* outgoingInterface = sr_get_interface(sr,nextHOPinterface->interface);
    if(!outgoingInterface){
/*      printf("No Outgoing Interface...\n");
*/      return;
    }
      
    struct sr_arpreq* request = sr_arpcache_queuereq(&sr->cache,ipHeader->ip_dst, packetCopy, len, outgoingInterface->name);

    if(request){

      handle_arpreq(sr,request);
/*      printf("Sending IP to next HOP....\n");
*/      fflush(stdout);
    } 
}
}







void send_icmp_host_unreachable(struct sr_instance* sr, uint8_t* oldPacket, unsigned int len, char* interface){
  uint8_t* packet = malloc(sizeof(sr_icmp_t3_hdr_t) + sizeof(sr_ip_hdr_t) + sizeof(sr_ethernet_hdr_t));
  memcpy(packet,oldPacket,sizeof(sr_ethernet_hdr_t) + sizeof(sr_ip_hdr_t));

  sr_ethernet_hdr_t* etherHeader = (sr_ethernet_hdr_t*)(packet);
  sr_ip_hdr_t*       ipHeader    = (sr_ip_hdr_t*)(packet+sizeof(sr_ethernet_hdr_t));
  sr_icmp_t3_hdr_t*  icmpHeader  = (sr_icmp_t3_hdr_t*)(packet+sizeof(sr_ethernet_hdr_t)+sizeof(sr_ip_hdr_t));


  icmpHeader->icmp_type = 3;
  icmpHeader->icmp_code = 0;
  icmpHeader->unused = 0;
  icmpHeader->next_mtu = 0;
  memcpy(icmpHeader->data, oldPacket+sizeof(sr_ethernet_hdr_t), ICMP_DATA_SIZE);
  icmpHeader->icmp_sum = 0;
  
  uint16_t icmpCheck = cksum(icmpHeader, sizeof(sr_icmp_t3_hdr_t));
  icmpHeader->icmp_sum = icmpCheck;

  struct sr_if* newInterface = sr_get_interface(sr,interface);

  ipHeader->ip_p = 1;
  ipHeader->ip_len = htons(sizeof(sr_ip_hdr_t) + sizeof(sr_icmp_t3_hdr_t));
  ipHeader->ip_dst = ipHeader->ip_src;
  ipHeader->ip_src = newInterface->ip;
  ipHeader->ip_sum = 0;
  ipHeader->ip_ttl = INIT_TTL;

  uint16_t ipSum = cksum(ipHeader, sizeof(sr_ip_hdr_t));
  ipHeader->ip_sum = ipSum;

  uint8_t* x = malloc(ETHER_ADDR_LEN);
  memcpy(x,etherHeader->ether_shost, ETHER_ADDR_LEN);
  memcpy(etherHeader->ether_shost, etherHeader->ether_dhost, ETHER_ADDR_LEN);
  memcpy(etherHeader->ether_dhost, x  , ETHER_ADDR_LEN);

  int length = sizeof(sr_icmp_t3_hdr_t)+sizeof(sr_ip_hdr_t)+sizeof(sr_ethernet_hdr_t);
/*  print_hdrs(packet,length);
printf("Sending ICMP HOst Unreachable...\n");  
*/  
  printf("Modified IP Header: \n");
  print_hdrs(packet,length);
  sr_send_packet(sr,packet,length, newInterface->name);  
}











void send_icmp_time_exceeded(struct sr_instance* sr, uint8_t* oldPacket, unsigned int len, char* interface) {
  uint8_t* packet = malloc(sizeof(sr_icmp_t3_hdr_t) + sizeof(sr_ip_hdr_t) + sizeof(sr_ethernet_hdr_t));
  memcpy(packet,oldPacket,sizeof(sr_ethernet_hdr_t) + sizeof(sr_ip_hdr_t));

  sr_ethernet_hdr_t* etherHeader = (sr_ethernet_hdr_t*)(packet);
  sr_ip_hdr_t*       ipHeader    = (sr_ip_hdr_t*)(packet+sizeof(sr_ethernet_hdr_t));
  sr_icmp_t3_hdr_t*  icmpHeader  = (sr_icmp_t3_hdr_t*)(packet+sizeof(sr_ethernet_hdr_t)+sizeof(sr_ip_hdr_t));


  icmpHeader->icmp_type = 11;
  icmpHeader->icmp_code = 0;
  memcpy(icmpHeader->data, oldPacket+sizeof(sr_ethernet_hdr_t), ICMP_DATA_SIZE);
  icmpHeader->icmp_sum = 0;
  
  uint16_t icmpCheck = cksum(icmpHeader, sizeof(sr_icmp_t3_hdr_t));
  icmpHeader->icmp_sum = icmpCheck;

  struct sr_if* newInterface = sr_get_interface(sr,interface);

  ipHeader->ip_p = 1;
  ipHeader->ip_len = htons(sizeof(sr_ip_hdr_t) + sizeof(sr_icmp_t3_hdr_t));
  ipHeader->ip_dst = ipHeader->ip_src;
  ipHeader->ip_src = newInterface->ip;
  ipHeader->ip_sum = 0;
  ipHeader->ip_ttl = INIT_TTL;

  uint16_t ipSum = cksum(ipHeader, sizeof(sr_ip_hdr_t));
  ipHeader->ip_sum = ipSum;

  uint8_t* x = malloc(ETHER_ADDR_LEN);
  memcpy(x,etherHeader->ether_shost, ETHER_ADDR_LEN);
  memcpy(etherHeader->ether_shost, etherHeader->ether_dhost, ETHER_ADDR_LEN);
  memcpy(etherHeader->ether_dhost, x  , ETHER_ADDR_LEN);

  int length = sizeof(sr_icmp_t3_hdr_t)+sizeof(sr_ip_hdr_t)+sizeof(sr_ethernet_hdr_t);
  printf("Modified IP Header: \n");
  print_hdrs(packet,length);
/*printf("Sending ICMP Time Exceeded...\n");  
*/
  sr_send_packet(sr,packet,length, newInterface->name);
}















uint8_t* generateICMPPacket(){
  /*Take Space for Whole Packet*/
  uint8_t* packet = malloc(sizeof(sr_icmp_t3_hdr_t) + sizeof(sr_ip_hdr_t) + sizeof(sr_ethernet_hdr_t));
  /*Setup Ethernet Header*/
  return packet;
}

int isPacketForMe(struct sr_instance* sr,uint32_t ip){
	struct sr_if* inter = sr->if_list;

	while(inter){
		if(inter->ip == ip){
			return 1;
		}
		inter = inter->next;
	}
	return 0;
}


void sr_HandleIPPacket(struct sr_instance* sr, uint8_t* packet, unsigned int len, char* interface) {

  uint8_t* oldPacket = malloc(len);
  memcpy(oldPacket,packet,len);
/* Ethernet header*/
  sr_ethernet_hdr_t* etherHeader = (sr_ethernet_hdr_t*)packet;
  struct sr_if* InterfaceStruct = sr_get_interface(sr,interface);
/*  Check IP packet Length*/
  if(len <= sizeof(sr_ethernet_hdr_t) +sizeof(sr_ip_hdr_t)){
/*    printf("Packet Length Too Short to be an IP Packet...\n");
*/    return;
  }
  /*validating IP Header*/
  sr_ip_hdr_t* ipPacket = (sr_ip_hdr_t*)(packet + sizeof(sr_ethernet_hdr_t));
  if(ipPacket->ip_v != 4){
/*    printf("Not an IPv4 Packet\n");
*/    return;
  } 
  /*Validating CheckSum*/
  uint16_t ipSum = ipPacket->ip_sum;
  ipPacket->ip_sum =0;
  if(cksum(packet+sizeof(sr_ethernet_hdr_t),sizeof(sr_ip_hdr_t)) !=  ipSum){
/*    printf("Checksum Not Valid!\n");
*/    return;
  } 

  /*Check if packet is For me*/
/*  ipPacket->ip_dst == InterfaceStruct->ip
*/if(isPacketForMe(sr,ipPacket->ip_dst)){
/*    printf("Packet for me\n");
*/    /*Check if it is ICMP*/
    if(ipPacket->ip_p == ip_protocol_icmp){
      /*If it is type 8 (Echo), reply ICMP Echo*/
      sr_icmp_hdr_t * icmpHeader = (sr_icmp_hdr_t*) (packet+sizeof(sr_ethernet_hdr_t)+sizeof(sr_ip_hdr_t));
      uint16_t* icmpCheck = icmpHeader->icmp_sum;
      icmpHeader->icmp_sum = 0;
      if(cksum(packet+sizeof(sr_ethernet_hdr_t)+sizeof(sr_ip_hdr_t), len - sizeof(sr_ethernet_hdr_t)-sizeof(sr_ip_hdr_t)) != icmpCheck){
/*        printf("ICMP Checksum is not valid!\n");
*/        return;
      }
      if(icmpHeader->icmp_type == 8) {
/*        printf("Echo MSG\n");
*/        
        /*Changing ICMP Header*/
        icmpHeader->icmp_type = 0;
        icmpHeader->icmp_code = 0;
        icmpHeader->icmp_sum = 0;
        uint16_t* icmpNewCheckSum = cksum(icmpHeader,len - sizeof(sr_ip_hdr_t) - sizeof(sr_ethernet_hdr_t));
        icmpHeader->icmp_sum = icmpNewCheckSum;

        /*Changing IP Header*/
        ipPacket->ip_ttl = INIT_TTL;
        ipPacket->ip_sum = 0;
        uint32_t sourceIP = ipPacket->ip_src;
        ipPacket->ip_src = ipPacket->ip_dst;
        ipPacket->ip_dst = sourceIP;
        uint16_t ipCheckSum = cksum(ipPacket, len - sizeof(sr_ethernet_hdr_t));
        ipPacket->ip_sum = ipCheckSum;

        /*Changing Ethernet header*/
        uint8_t *x = malloc(ETHER_ADDR_LEN);
        memcpy(x,etherHeader->ether_shost, ETHER_ADDR_LEN);
        memcpy(etherHeader->ether_shost,etherHeader->ether_dhost, ETHER_ADDR_LEN);
        memcpy(etherHeader->ether_dhost, x,ETHER_ADDR_LEN);

        struct sr_rt* routingEntry = getRoutingTableEntry(sr,sourceIP);
        struct sr_if* newInterface = sr_get_interface(sr,routingEntry->interface);
/*        print_hdrs(etherHeader,len);
      printf("Sending ICMP Echo Reply..\n");

*/      printf("Modified IP Header: \n");
        print_hdrs(etherHeader,len);  
        sr_send_packet(sr,etherHeader,len,newInterface->name);

      } else {
/*        printf("ICMP for me but not an Echo Request\n");
*/      }
    } else {
      /*Send ICMP PORT UNREACHABLE REPLY*/
      uint8_t* newPacket = generateICMPPacket();

      /*Changing ICMP Header*/
      sr_icmp_t3_hdr_t* icmpHeader = (sr_icmp_t3_hdr_t*)(newPacket+sizeof(sr_ethernet_hdr_t)+sizeof(sr_ip_hdr_t));
      icmpHeader->icmp_type = 3;
      icmpHeader->icmp_code = 3;
      icmpHeader->icmp_sum = 0;
      icmpHeader->unused = 0;
      icmpHeader->next_mtu = 0;
      memcpy(icmpHeader->data, oldPacket+sizeof(sr_ethernet_hdr_t),ICMP_DATA_SIZE);
      uint16_t* icmpNewCheckSum = cksum(icmpHeader,sizeof(sr_icmp_t3_hdr_t));
      icmpHeader->icmp_sum = icmpNewCheckSum;

      /*Changing IP Header*/
      sr_ip_hdr_t* ipNewPacket = (sr_ip_hdr_t*)(newPacket+sizeof(sr_ethernet_hdr_t));
      ipNewPacket->ip_ttl = INIT_TTL;
      ipNewPacket->ip_sum = 0;
      ipNewPacket->ip_v = 4;
      ipNewPacket->ip_hl = 5;
      ipNewPacket->ip_src = ipPacket->ip_dst;
      ipNewPacket->ip_dst = ipPacket->ip_src;
      ipNewPacket->ip_len = htons(sizeof(sr_ip_hdr_t)+sizeof(sr_icmp_t3_hdr_t));
      ipNewPacket->ip_tos = ipPacket->ip_tos;
      ipNewPacket->ip_id = ipPacket->ip_id;
      ipNewPacket->ip_p = 1;
      ipNewPacket->ip_off = ipPacket->ip_off;
      uint16_t ipCheckSum = cksum(ipNewPacket, sizeof(sr_ip_hdr_t));
      ipNewPacket->ip_sum = ipCheckSum;


      /*Changing Ethernet header*/
      sr_ethernet_hdr_t* newEtherHeader = (sr_ethernet_hdr_t*)newPacket;
      uint8_t *x = malloc(ETHER_ADDR_LEN);
      memcpy(newEtherHeader->ether_shost, etherHeader->ether_dhost, ETHER_ADDR_LEN);
      memcpy(newEtherHeader->ether_dhost, etherHeader->ether_shost,ETHER_ADDR_LEN);
      newEtherHeader->ether_type = etherHeader->ether_type;

      struct sr_rt* routingEntry = getRoutingTableEntry(sr,ipNewPacket->ip_dst);
      struct sr_if* newInterface = sr_get_interface(sr,routingEntry->interface);
/*      print_hdrs(newEtherHeader,len);
*/    printf("Modified IP Header: \n");
        print_hdrs(newEtherHeader,sizeof(sr_ethernet_hdr_t)+sizeof(sr_ip_hdr_t)+sizeof(sr_icmp_t3_hdr_t));    
      sr_send_packet(sr,newEtherHeader,sizeof(sr_ethernet_hdr_t)+sizeof(sr_ip_hdr_t)+sizeof(sr_icmp_t3_hdr_t),newInterface->name);
    }
  } else if(getRoutingTableEntry(sr,ipPacket->ip_dst) == NULL) {
/*    printf("Packet Not for me\n");
*/    send_icmp_host_unreachable(sr,oldPacket,len,interface);
  } else {
    /*handling my interfaces' IP Request*/

    /*Checking TTL*/
    if(ipPacket->ip_ttl <= 1) {
      /*Send ICMP Time Exceeded*/
      send_icmp_time_exceeded(sr,oldPacket,len,interface);
    } else {
      /*Forward Packet*/
      send_ip_next_hop(sr,oldPacket,len,interface);
    }
  }

}



















int isArpReplyPacket(uint8_t* buf){
    sr_arp_hdr_t *arp_hdr = (sr_arp_hdr_t *)(buf);
    return (ntohs(arp_hdr->ar_op) == arp_op_reply) ? 1 : 0;
    
}
void swap(char* a, char* b){
  char* c = a;
  a = b;
  b = c;
}
void handleARPRequest(struct sr_instance* sr, uint8_t* packet, unsigned int len, char* interface){
    sr_arp_hdr_t *arp_hdr = (sr_arp_hdr_t *)(packet + sizeof(sr_ethernet_hdr_t));
    arp_hdr->ar_op = htons(arp_op_reply);
    
    uint32_t sourceIP =  arp_hdr->ar_sip;
    arp_hdr->ar_sip = arp_hdr->ar_tip;
    arp_hdr->ar_tip = sourceIP;

    memcpy(arp_hdr->ar_tha, arp_hdr->ar_sha ,sizeof(arp_hdr->ar_sha));

    
    sr_ethernet_hdr_t *ehdr = (sr_ethernet_hdr_t *)packet;

    char * xxx = malloc(sizeof(ehdr->ether_shost));
    memcpy(ehdr->ether_dhost, ehdr->ether_shost,  sizeof(ehdr->ether_shost));
    struct sr_rt* routingEntry = getRoutingTableEntry(sr,sourceIP);
    struct sr_if* newInterface = sr_get_interface(sr,routingEntry->interface);

    memcpy(arp_hdr->ar_sha,newInterface->addr, ETHER_ADDR_LEN );

    memcpy(ehdr->ether_shost, newInterface->addr, ETHER_ADDR_LEN);

/*    print_hdrs(packet,len);
*/   /*printf("Sending ARP Reply to ARP Request....\n");*/  
    sr_send_packet(sr,packet,42,newInterface->name);
}

void sr_handleARPPacket(struct sr_instance* sr, uint8_t* packet, unsigned int len, char* interface) {
  if(len >= sizeof(sr_ethernet_hdr_t) + sizeof(sr_arp_hdr_t) ){
/*    printf("Length Verified\n");
*/    if(sr_arp_not_for_us(sr,packet,len,interface) == 0){
      if(isArpReplyPacket(packet + sizeof(sr_ethernet_hdr_t))){
        /*Handle ARP Reply packet*/
/*        printf("Handling ARP REPLY..........\n");
*/        handleArpReply(sr,packet,len,interface);
      } else {
/*        printf("Handling ARP Request.......");
*/        uint8_t* copyPack = malloc(len);
        memcpy(copyPack,packet,len);
        handleARPRequest(sr,copyPack,len,interface);
      }
    } else {
/*      printf("Arp Req/Rep is not for me....\n");
*/    }
  } else {
/*    printf("Packet is not Proper Arp Packet. Length too minimum...\n");
*/  }
}

void sr_handlepacket(struct sr_instance* sr,
        uint8_t * packet/* lent */,
        unsigned int len,
        char* interface/* lent */)
{
  /* REQUIRES */
  assert(sr);
  assert(packet);
  assert(interface);

  printf("*** -> Received packet of length %d \n",len);
  uint8_t* packetCopy = malloc(len);
  memcpy(packetCopy,packet,len);
  print_hdrs(packet,len);

  /* fill in code here */
  sr_ethernet_hdr_t *ehdr = (sr_ethernet_hdr_t *)packetCopy;
  uint16_t type = ethertype(packetCopy);
/*  printf("Type: %d\n", type);
*/  if(ntohs(ehdr->ether_type) == 2054){
/*    printf("Handling Arp packet\n" );
*/    sr_handleARPPacket(sr,packetCopy,len,interface);
  } 
  else if(ntohs(ehdr->ether_type) == ethertype_ip){
/*    printf("Handling IP packet\n");
*/    sr_HandleIPPacket(sr,packetCopy,len,interface);

  } else {
/*    printf("Packet is not recognizable..Dropping Packet\n");
*/  }

}/* end sr_ForwardPacket */


int  sr_arp_not_for_us(struct sr_instance* sr,
                           uint8_t * packet /* lent */,
                           unsigned int len,
                           char* interface  /* lent */)
{
    struct sr_if* iface = sr_get_interface(sr, interface);
    struct sr_ethernet_hdr* e_hdr = 0;
    struct sr_arp_hdr*       a_hdr = 0;

    if (len < sizeof(struct sr_ethernet_hdr) + sizeof(struct sr_arp_hdr) )
    { return 0; }

    assert(iface);

    e_hdr = (struct sr_ethernet_hdr*)packet;
    a_hdr = (struct sr_arp_hdr*)(packet + sizeof(struct sr_ethernet_hdr));

    if ( (e_hdr->ether_type == htons(ethertype_arp)) &&
            (a_hdr->ar_tip     != iface->ip ) )
    { return 1; }

    return 0;
} /* -- sr_arp_rep_not_for_us -- */


