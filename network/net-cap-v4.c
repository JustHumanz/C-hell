#include <arpa/inet.h>
#include <net/ethernet.h>
#include <pcap/pcap.h>
#include <stdio.h>
#include <pcap.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <sys/types.h>
#include <string.h>

#define SIZE_ETHERNET 14
#define SNAP_LEN 1518

struct sniff_ethernet {
    u_char eth_dhst[ETHER_ADDR_LEN];
    u_char eth_shst[ETHER_ADDR_LEN];
    u_short eth_type;
};

struct sniff_ip {
    u_char ip_vhl;
    u_char ip_tos;
    u_short ip_len;
    u_short ip_id;
    u_short ip_off;
    #define IP_RF 0x8000
    #define IP_DF 0x4000 
    #define IP_MF 0x2000 
    #define IP_OFFMASK 0x1fff
    u_char ip_ttl;
    u_char ip_p;
    u_short ip_sum;
    struct in_addr ip_src,ip_dst;
};
#define IP_HL(ip) (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip) (((ip)->ip_vhl) >> 4)

typedef u_int tcp_seq;

struct sniff_tcp {
    u_short th_sport;
    u_short th_dport;
    tcp_seq th_seq;
    tcp_seq th_ack;
    u_char th_offx2;
    # define TH_OFF(th) (((th)->th_offx2 & 0xf0) >> 4)
    u_char th_flags;
    #define TH_FIN 0x01;
    #define TH_SYN 0x02;
    #define TH_RST 0x03;
    #define TH_PUSH 0x04;
    #define TH_ACK 0x05;
    #define TH_URG 0x06;
    #define TH_ECE 0x07;
    #define TH_CWR 0x08;
    #define TH_FLAGS (TH_FIN|TH_SYN|TH_RST|TH_PUSH|TH_ACK|TH_URG|TH_ECE|TH_CWR)
    u_short th_win;
    u_short th_sum;
    u_short th_urp;
};

char * ip_local;

#include <errno.h>
#include <time.h>

int msleep(long tms)
{
    struct timespec ts;
    int ret;

    if (tms < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = tms / 1000;
    ts.tv_nsec = (tms % 1000) * 1000000;

    do {
        ret = nanosleep(&ts, &ts);
    } while (ret && errno == EINTR);

    return ret;
}

void pkt_hdr(u_char *args,const struct pcap_pkthdr *hdr,const u_char *pkt){
    static int count = 1;

    const struct sniff_ethernet *eth;
    const struct sniff_ip *ip;
    const struct sniff_tcp *tcp;
    const char *payload;

    int size_ip,size_tcp,size_payload;

    printf("Packet number %d:\n",count);
    count++;

    eth = (struct sniff_ethernet*)(pkt);

    ip = (struct sniff_ip*)(pkt + SIZE_ETHERNET);
    size_ip = IP_HL(ip)*4;
    if (size_ip < 20){
        printf("Invalid IP header len: %u",size_ip);
        return;
    }

    printf("From: %s\n",inet_ntoa(ip->ip_src));
    printf("To: %s\n",inet_ntoa(ip->ip_dst));

    if (ip->ip_p == IPPROTO_TCP) {
        printf("TCP pkt\n");
    } else if (ip->ip_p == IPPROTO_UDP) {
        printf("UDP pkt\n");
        return;
    } else if (ip->ip_p == IPPROTO_ICMP) {
        printf("ICMP pkt\n");
        return;        
    } else if (ip->ip_p == IPPROTO_IP) {
        printf("IP pkt\n");
        return;        
    } else {
        printf("IDK what the fvck this pkt\n");
        return;        
    }
    
    tcp = (struct sniff_tcp*)(pkt + SIZE_ETHERNET + size_ip);
    size_tcp = TH_OFF(tcp)*4;
    if (size_tcp < 20) {
        printf("Invalid TCP header len: %u bytes\n",size_tcp);
        return;
    }

    printf("Src Port: %d\n",ntohs(tcp->th_sport));
    printf("Dst Port: %d\n",ntohs(tcp->th_dport));

    //TODO
    //Make sure the respose is TH_RST
    if (tcp->th_flags & 0x03 && inet_ntoa(ip->ip_dst) == ip_local && ntohs(tcp->th_sport) == 443){
        printf("IP %s have blocked by kominfo\n",inet_ntoa(ip->ip_dst));
        msleep(5000);
    }

    payload = (char *)(pkt+SIZE_ETHERNET+size_ip+size_tcp);

    size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);

    if (size_payload > 0) {
        printf("Payload (%d bytes):\n",size_payload);
    }

    return;
}

int main(int argc,char *argv[]){
    pcap_if_t *dev = NULL;
    char *dev_name;
    char err_buff[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    int snap_len = 1024;
    u_char *my_arg = NULL;

    if(pcap_findalldevs(&dev, err_buff) == -1){
        fprintf(stderr, "There is a problem with pcap_findalldevs: %s\n", err_buff);
        return -1;        
    }

    while (dev != NULL) {
        if (strcmp(dev->name, argv[1])) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)dev->addresses->addr;
            void *addr = &(ipv4->sin_addr);
            ip_local = inet_ntoa(ipv4->sin_addr);

            dev_name = dev->name;
            break;
        }
        dev = dev->next;
    }

    handle = pcap_open_live(dev_name, snap_len, 0, 10000, err_buff);
    pcap_loop(handle, 0, pkt_hdr, my_arg);
    pcap_freealldevs(dev);
    return 0;
}