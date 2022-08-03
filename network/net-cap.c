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

int main(int argc,char * argv[]){
    pcap_if_t *dev = NULL;
    char *dev_name;
    char err_buff[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

    const u_char *pkt;
    struct pcap_pkthdr packet_header;
    int pkt_cnt_lmt = 1;
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

    handle = pcap_open_live(dev_name, BUFSIZ, pkt_cnt_lmt, timeout_lmt, err_buff);

    pkt = pcap_next(handle, &packet_header);
    if (pkt == NULL) {
        printf("No packet found.\n");
        return 2;        
    }
    
    prt_pkt_inf(pkt, packet_header);


    pcap_freealldevs(dev);
    return 0;

}