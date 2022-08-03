#include <pcap.h>
#include <pcap/pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main(int argc,char **argv){
    pcap_if_t *dev = NULL;
    char err_buff[PCAP_ERRBUF_SIZE];
    char ipstr[INET6_ADDRSTRLEN],netmsk [INET6_ADDRSTRLEN];

    if(pcap_findalldevs(&dev, err_buff) == -1){
        fprintf(stderr, "There is a problem with pcap_findalldevs: %s\n", err_buff);
        return -1;        
    }


    while (dev != NULL) {
        pcap_addr_t *dev_addr;
        for (dev_addr = dev->addresses; dev_addr != NULL; dev_addr = dev_addr->next) {
            if (dev_addr->addr->sa_family == AF_INET && dev_addr->addr && dev_addr->netmask) {
                struct sockaddr_in *ipv4 = (struct sockaddr_in *)dev_addr->addr;
                void *addr = &(ipv4->sin_addr);
                inet_ntop(dev_addr->addr->sa_family,addr,ipstr,sizeof(ipstr));

                struct sockaddr_in *net_mask = (struct sockaddr_in *)dev_addr->netmask;
                void *mask = &(net_mask->sin_addr);                
                inet_ntop(dev_addr->addr->sa_family,mask,netmsk,sizeof(netmsk));

                printf("Found a device %s on address %s netmask %s\n", dev->name, ipstr,netmsk);
            }
        }        
        dev = dev->next;
    }
    pcap_freealldevs(dev);
        
    return 0;
}