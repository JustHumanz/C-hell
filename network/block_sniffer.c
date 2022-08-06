#include <linux/if_ether.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pcap/pcap.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>

char * ip_local;
int total;
struct sockaddr_in source,dest;

void pkt_hdr(u_char *args,const struct pcap_pkthdr *hdr,const u_char *pkt){
    int size = hdr->len;
	struct iphdr *iph = (struct iphdr*)(pkt + sizeof(struct ethhdr));
    int iphlen = iph->ihl*4;

    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;

    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;    
    ++total;

    if (iph->protocol == IPPROTO_TCP) {
        struct iphdr *iph = (struct iphdr *)(pkt + sizeof(struct ethhdr));
        int iphdrlen = iph->ihl*4;

        struct tcphdr *tcph = (struct tcphdr *)(pkt + iphdrlen +sizeof(struct ethhdr));
        int header_size = sizeof(struct ethhdr)+iphdrlen+tcph->doff*4;
        if (ntohs(tcph->source) ==443 || ntohs(tcph->dest)==443)  {
            int pktflag = (int)tcph->th_flags;

            printf("==============================================================\n");
            printf("TCP Header Length       : %d DWORDS or %d BYTES\n",(unsigned int)tcph->doff,(unsigned int)tcph->doff*4);
            printf("From                    : %s:%d\n",inet_ntoa(source.sin_addr),ntohs(tcph->source));
            printf("To                      : %s:%d\n",inet_ntoa(dest.sin_addr),ntohs(tcph->dest));
            printf("Flag                    : %d\n",pktflag);
            printf("Window                  : %d\n",ntohs(tcph->window));
            if (pktflag == 20) {
                printf("IP: %s:%d blocked by kominfo\n",inet_ntoa(source.sin_addr),ntohs(tcph->source));
                //TODO
                //Reroute the ip into tunnel interfaces
            }
        }        
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
        if (strcmp(dev->name, argv[1]) == 0) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)dev->addresses->addr;
            void *addr = &(ipv4->sin_addr);
            ip_local = inet_ntoa(ipv4->sin_addr);

            dev_name = dev->name;
            break;
        }
        dev = dev->next;
    }

    handle = pcap_open_live(dev_name, snap_len, 0, 10000, err_buff);
	if (handle == NULL) {
		fprintf(stderr, "Couldn't open device %s : %s\n" , dev_name , err_buff);
		exit(1);
	}    

    pcap_loop(handle, 0, pkt_hdr, my_arg);
    pcap_freealldevs(dev);
    return 0;
}