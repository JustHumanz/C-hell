#include <pcap/pcap.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <pcap.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <string.h>

void prt_pkt_inf(const u_char *pkt,struct pcap_pkthdr packet_header) {
    printf("Packet capture length: %d\n", packet_header.caplen);
    printf("Packet total length %d\n", packet_header.len);    
}


void pkt_handler(u_char *args,const struct pcap_pkthdr *packet_header,const u_char *pkt_bdy){
    prt_pkt_inf(pkt_bdy,*packet_header);
}

int main(int argc,char * argv[]){
    pcap_if_t *dev = NULL;
    char *dev_name;
    char err_buff[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

    const u_char *pkt;
    struct pcap_pkthdr packet_header;
    int timeout_lmt = 100000;

    if(pcap_findalldevs(&dev, err_buff) == -1){
        fprintf(stderr, "There is a problem with pcap_findalldevs: %s\n", err_buff);
        return -1;        
    }

    while (dev != NULL) {
        if (strcmp(dev->name, argv[1])) {
            dev_name = dev->name;
            break;
        }
        dev = dev->next;
    }

    handle = pcap_open_live(dev_name, BUFSIZ, 0, timeout_lmt, err_buff);
    if (handle == NULL) {
        fprintf(stderr,"Could not open device %s: %s\n", dev_name, err_buff);
    }

    pcap_loop(handle, 0, pkt_handler, NULL);
    pcap_freealldevs(dev);
    return 0;

}