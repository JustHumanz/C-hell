#include <net/ethernet.h>
#include <pcap/pcap.h>
#include <stdio.h>
#include <pcap.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <sys/types.h>
#include <string.h>

void pkt_hdr(u_char *args,const struct pcap_pkthdr *hdr,const u_char *pkt){
    struct ether_header *eth_hdr;
    eth_hdr = (struct ether_header *)pkt;
    if (ntohs(eth_hdr->ether_type) != ETHERTYPE_IP) {
        printf("Not ip pkt");
        return;
    }

    printf("Packet capture length: %d\n", hdr->caplen);
    printf("Packet total length %d\n", hdr->len);    

    const u_char *ip_hdr,*tcp_hdr,*payload;

    int eth_hdr_len = 14,ip_hdr_len,tcp_hdr_len,payload_len;

    ip_hdr = pkt+eth_hdr_len;
    ip_hdr_len = ((*ip_hdr)&0x0F);
    ip_hdr_len = ip_hdr_len*4;

    printf("IP header len (IHL) in bytes: %d\n",ip_hdr_len);

    u_char protocol = *(ip_hdr +9);
    if (protocol != IPPROTO_TCP) {
        printf("Not tcp,skip\n");
        return;
    }

    tcp_hdr= pkt + eth_hdr_len +ip_hdr_len;
    tcp_hdr_len = ((*(tcp_hdr+12)) &0XF0) >> 4;
    tcp_hdr_len = tcp_hdr_len*4;


    printf("TCP hdr in bytes: %d\n",tcp_hdr_len);

    int total_hdr_size = eth_hdr_len+ip_hdr_len+tcp_hdr_len;
    printf("Size of all headers: %d\n",total_hdr_size);

    payload_len = hdr->caplen - (eth_hdr_len+ip_hdr_len+tcp_hdr_len);
    printf("Payload size: %d\n",payload_len);
    
    payload = pkt+total_hdr_size;
    printf("Mem address: %p\n",payload);

    /*
    if (payload_len > 0) {
        const u_char *tmp_ptr = payload;
        int byt_cnt= 0;
        while (byt_cnt++ < payload_len) {
            printf("%c",*tmp_ptr);
            tmp_ptr++;
        }
        printf("\n");
    }
    */
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