#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <string.h>
#include <limits.h>
#include "sr_arpcache.h"
#include "sr_router.h"
#include "sr_if.h"
#include "sr_protocol.h"
#include "sr_rt.h"
#define MAX_NUM_OF_ARP_REQ 10



uint8_t DEFAULT_MAC[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};


/* 
  This function gets called every second. For each request sent out, we keep
  checking whether we should resend an request or destroy the arp request.
  See the comments in the header file for an idea of what it should look like.
*/
/*  function handle_arpreq(req):
       if difftime(now, req->sent) > 1.0
           if req->times_sent >= 5:
               send icmp host unreachable to source addr of all pkts waiting
                 on this request
               arpreq_destroy(req)
           else:
               send arp request
               req->sent = now
               req->times_sent++
*/
void handle_arpreq(struct sr_instance* sr, struct sr_arpreq* eachRequest) {
    time_t curtime = time(NULL);
    if ( difftime(curtime, eachRequest->sent) > 0 ) {
        if (eachRequest->times_sent >= MAX_NUM_OF_ARP_REQ) {
/*            fprintf(stderr, "Host Unreachable..\n");
*/            sr_icmp_req_send(sr, eachRequest);
            sr_arpreq_destroy(&sr->cache , eachRequest);
        } else {
/*            send ARP Request */ 
/*            printf("Sending ARP Requests\n");
*/            sendARPRequest(sr,eachRequest);
            eachRequest->sent = curtime;
            eachRequest->times_sent++;
        }
    }
}

void sendARPRequest(struct sr_instance* sr, struct sr_arpreq* eachRequest){
    uint8_t* packet = malloc(sizeof(sr_ethernet_hdr_t)+sizeof(sr_arp_hdr_t));

    sr_ethernet_hdr_t* etherHeader = (sr_ethernet_hdr_t*) packet;
    sr_arp_hdr_t* arpHeader = (sr_arp_hdr_t*)(packet+sizeof(sr_ethernet_hdr_t));

    struct  sr_rt* routingEntry = getRoutingTableEntry(sr,eachRequest->ip);
    struct sr_if * newInterface = sr_get_interface(sr, routingEntry->interface);

    memcpy(etherHeader->ether_dhost, DEFAULT_MAC, ETHER_ADDR_LEN);
    memcpy(etherHeader->ether_shost, newInterface->addr, ETHER_ADDR_LEN);
    etherHeader->ether_type = htons(ethertype_arp);
 
    arpHeader->ar_hrd = htons(arp_hrd_ethernet);
    arpHeader->ar_pro = htons(ethertype_ip);
    arpHeader->ar_hln = ETHER_ADDR_LEN;
    arpHeader->ar_pln = 4;
    arpHeader->ar_op = htons(arp_op_request);
    memcpy(arpHeader->ar_sha ,newInterface->addr, ETHER_ADDR_LEN);
    memcpy(arpHeader->ar_tha ,DEFAULT_MAC, ETHER_ADDR_LEN);
    arpHeader->ar_sip = newInterface->ip;
    arpHeader->ar_tip = eachRequest->ip;

/*    print_hdrs(packet,sizeof(sr_ethernet_hdr_t)+sizeof(sr_arp_hdr_t));
*/    int x = sr_send_packet(sr,packet,sizeof(sr_ethernet_hdr_t)+sizeof(sr_arp_hdr_t), newInterface->name);
    if(x){
/*        printf("arp packet not sent...\n");
*/    }

}
void sr_icmp_req_send(struct sr_instance* sr,  struct sr_arpreq* request) {

    struct sr_packet* eachRequest = request->packets;
    unsigned int length = sizeof(sr_ethernet_hdr_t) +  sizeof(sr_icmp_t3_hdr_t) + sizeof(sr_ip_hdr_t) ;
    
    while(eachRequest)  {
/*        printf("Sending ICMP REply Destination Unreachable\n");
*/        struct sr_packet* nextPacket = eachRequest->next;
        uint8_t* newPacket = malloc(length);
        memcpy(newPacket, eachRequest->buf ,sizeof(sr_ethernet_hdr_t) + sizeof(sr_ip_hdr_t));

        sr_ethernet_hdr_t* etherHeader = (sr_ethernet_hdr_t*)(newPacket);
        sr_ip_hdr_t*       ipHeader    = (sr_ip_hdr_t*)(newPacket+sizeof(sr_ethernet_hdr_t));
        sr_icmp_t3_hdr_t*  icmpHeader  = (sr_icmp_t3_hdr_t*)(newPacket+sizeof(sr_ethernet_hdr_t)+sizeof(sr_ip_hdr_t));


        icmpHeader->icmp_type = 3;
        icmpHeader->icmp_code = 1;
        icmpHeader->unused = 0;
        icmpHeader->next_mtu = 0;
        memcpy(icmpHeader->data, eachRequest->buf+sizeof(sr_ethernet_hdr_t), ICMP_DATA_SIZE);
        icmpHeader->icmp_sum = 0;

        uint16_t icmpCheck = cksum(icmpHeader, sizeof(sr_icmp_t3_hdr_t));
        icmpHeader->icmp_sum = icmpCheck;


        struct sr_rt* Enrty = getRoutingTableEntry(sr, ipHeader->ip_src );
        struct sr_if* newInterface = sr_get_interface(sr , Enrty->interface );

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
        memcpy(etherHeader->ether_shost, newInterface->addr, ETHER_ADDR_LEN);
        memcpy(etherHeader->ether_dhost, x  , ETHER_ADDR_LEN);

        int length = sizeof(sr_icmp_t3_hdr_t)+sizeof(sr_ip_hdr_t)+sizeof(sr_ethernet_hdr_t);
/*        print_hdrs(newPacket,length);
*/        sr_send_packet(sr,newPacket,length, newInterface->name); 

        eachRequest = nextPacket;
    }
/*    printf("Sent ICMP Requests..\n");
*/}
void sr_arpcache_sweepreqs(struct sr_instance *sr) { 
    /* Fill this in */
    struct sr_arpreq* eachRequest = sr->cache.requests;
    struct sr_arpreq* nextRequest;
    while(eachRequest != NULL){
        nextRequest = eachRequest->next;
        handle_arpreq(sr,eachRequest);
        eachRequest = nextRequest;
    }
}

/* You should not need to touch the rest of this code. */

/* Checks if an IP->MAC mapping is in the cache. IP is in network byte order.
   You must free the returned structure if it is not NULL. */
struct sr_arpentry *sr_arpcache_lookup(struct sr_arpcache *cache, uint32_t ip) {
    pthread_mutex_lock(&(cache->lock));
    
    struct sr_arpentry *entry = NULL, *copy = NULL;
    
    int i;
    for (i = 0; i < SR_ARPCACHE_SZ; i++) {
        if ((cache->entries[i].valid) && (cache->entries[i].ip == ip)) {
            entry = &(cache->entries[i]);
        }
    }
    
    /* Must return a copy b/c another thread could jump in and modify
       table after we return. */
    if (entry) {
        copy = (struct sr_arpentry *) malloc(sizeof(struct sr_arpentry));
        memcpy(copy, entry, sizeof(struct sr_arpentry));
    }
        
    pthread_mutex_unlock(&(cache->lock));
    
    return copy;
}

/* Adds an ARP request to the ARP request queue. If the request is already on
   the queue, adds the packet to the linked list of packets for this sr_arpreq
   that corresponds to this ARP request. You should free the passed *packet.
   
   A pointer to the ARP request is returned; it should not be freed. The caller
   can remove the ARP request from the queue by calling sr_arpreq_destroy. */
struct sr_arpreq *sr_arpcache_queuereq(struct sr_arpcache *cache,
                                       uint32_t ip,
                                       uint8_t *packet,           /* borrowed */
                                       unsigned int packet_len,
                                       char *iface)
{
    pthread_mutex_lock(&(cache->lock));
    
    struct sr_arpreq *req;
    for (req = cache->requests; req != NULL; req = req->next) {
        if (req->ip == ip) {
            break;
        }
    }
    
    /* If the IP wasn't found, add it */
    if (!req) {
        req = (struct sr_arpreq *) calloc(1, sizeof(struct sr_arpreq));
        req->ip = ip;
        req->next = cache->requests;
        cache->requests = req;
    }
    
    /* Add the packet to the list of packets for this request */
    if (packet && packet_len && iface) {
        struct sr_packet *new_pkt = (struct sr_packet *)malloc(sizeof(struct sr_packet));
        
        new_pkt->buf = (uint8_t *)malloc(packet_len);
        memcpy(new_pkt->buf, packet, packet_len);
        new_pkt->len = packet_len;
		new_pkt->iface = (char *)malloc(sr_IFACE_NAMELEN);
        strncpy(new_pkt->iface, iface, sr_IFACE_NAMELEN);
        new_pkt->next = req->packets;
        req->packets = new_pkt;
    }
    
    pthread_mutex_unlock(&(cache->lock));
    
    return req;
}

/* This method performs two functions:
   1) Looks up this IP in the request queue. If it is found, returns a pointer
      to the sr_arpreq with this IP. Otherwise, returns NULL.
   2) Inserts this IP to MAC mapping in the cache, and marks it valid. */
struct sr_arpreq *sr_arpcache_insert(struct sr_arpcache *cache,
                                     unsigned char *mac,
                                     uint32_t ip)
{
    pthread_mutex_lock(&(cache->lock));
    
    struct sr_arpreq *req, *prev = NULL, *next = NULL; 
    for (req = cache->requests; req != NULL; req = req->next) {
        if (req->ip == ip) {            
            if (prev) {
                next = req->next;
                prev->next = next;
            } 
            else {
                next = req->next;
                cache->requests = next;
            }
            
            break;
        }
        prev = req;
    }
    
    int i;
    for (i = 0; i < SR_ARPCACHE_SZ; i++) {
        if (!(cache->entries[i].valid))
            break;
    }
    
    if (i != SR_ARPCACHE_SZ) {
        memcpy(cache->entries[i].mac, mac, 6);
        cache->entries[i].ip = ip;
        cache->entries[i].added = time(NULL);
        cache->entries[i].valid = 1;
    }
    
    pthread_mutex_unlock(&(cache->lock));
    
    return req;
}

/* Frees all memory associated with this arp request entry. If this arp request
   entry is on the arp request queue, it is removed from the queue. */
void sr_arpreq_destroy(struct sr_arpcache *cache, struct sr_arpreq *entry) {
    pthread_mutex_lock(&(cache->lock));
    
    if (entry) {
        struct sr_arpreq *req, *prev = NULL, *next = NULL; 
        for (req = cache->requests; req != NULL; req = req->next) {
            if (req == entry) {                
                if (prev) {
                    next = req->next;
                    prev->next = next;
                } 
                else {
                    next = req->next;
                    cache->requests = next;
                }
                
                break;
            }
            prev = req;
        }
        
        struct sr_packet *pkt, *nxt;
        
        for (pkt = entry->packets; pkt; pkt = nxt) {
            nxt = pkt->next;
            if (pkt->buf)
                free(pkt->buf);
            if (pkt->iface)
                free(pkt->iface);
            free(pkt);
        }
        
        free(entry);
    }
    
    pthread_mutex_unlock(&(cache->lock));
}

/* Prints out the ARP table. */
void sr_arpcache_dump(struct sr_arpcache *cache) {
    fprintf(stderr, "\nMAC            IP         ADDED                      VALID\n");
    fprintf(stderr, "-----------------------------------------------------------\n");
    
    int i;
    for (i = 0; i < SR_ARPCACHE_SZ; i++) {
        struct sr_arpentry *cur = &(cache->entries[i]);
        unsigned char *mac = cur->mac;
        fprintf(stderr, "%.1x%.1x%.1x%.1x%.1x%.1x   %.8x   %.24s   %d\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ntohl(cur->ip), ctime(&(cur->added)), cur->valid);
    }
    
    fprintf(stderr, "\n");
}

/* Initialize table + table lock. Returns 0 on success. */
int sr_arpcache_init(struct sr_arpcache *cache) {  
    /* Seed RNG to kick out a random entry if all entries full. */
    srand(time(NULL));
    
    /* Invalidate all entries */
    memset(cache->entries, 0, sizeof(cache->entries));
    cache->requests = NULL;
    
    /* Acquire mutex lock */
    pthread_mutexattr_init(&(cache->attr));
    pthread_mutexattr_settype(&(cache->attr), PTHREAD_MUTEX_RECURSIVE);
    int success = pthread_mutex_init(&(cache->lock), &(cache->attr));
    
    return success;
}

/* Destroys table + table lock. Returns 0 on success. */
int sr_arpcache_destroy(struct sr_arpcache *cache) {
    return pthread_mutex_destroy(&(cache->lock)) && pthread_mutexattr_destroy(&(cache->attr));
}

/* Thread which sweeps through the cache and invalidates entries that were added
   more than SR_ARPCACHE_TO seconds ago. */
void *sr_arpcache_timeout(void *sr_ptr) {
    struct sr_instance *sr = sr_ptr;
    struct sr_arpcache *cache = &(sr->cache);
    
    while (1) {
        sleep(1.0);
        
        pthread_mutex_lock(&(cache->lock));
    
        time_t curtime = time(NULL);
        
        int i;    
        for (i = 0; i < SR_ARPCACHE_SZ; i++) {
            if ((cache->entries[i].valid) && (difftime(curtime,cache->entries[i].added) > SR_ARPCACHE_TO)) {
                cache->entries[i].valid = 0;
            }
        }
        
        sr_arpcache_sweepreqs(sr);

        pthread_mutex_unlock(&(cache->lock));
    }
    
    return NULL;
}

