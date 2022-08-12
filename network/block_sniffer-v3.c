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
#include <linux/netlink.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/rtnetlink.h>

int open_netlink(){
    struct sockaddr_nl saddr;

    int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);

    if (sock < 0 ) {
        perror("Failed to open netlink");
        return -1;
    }

    memset(&saddr, 0, sizeof(saddr));
    return  sock;
}

#define NLMSG_TAIL(nmsg) ((struct rtattr *) (((void *) (nmsg)) + NLMSG_ALIGN((nmsg)->nlmsg_len)))


int rtattr_add(struct nlmsghdr *n,int maxlen,int type,const void *data,int alen){
    int len = RTA_LENGTH(alen);
    struct rtattr *rta;

    if (NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(len) > maxlen) {
        fprintf(stderr, "rtattr_add error: message exceeded bound of %d\n",maxlen);
        return -1;
    }

    rta = NLMSG_TAIL(n);
    rta->rta_type = type;
    rta->rta_len = len;

    if (alen) {
        memcpy(RTA_DATA(rta),data, alen);
    }

    n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len + RTA_ALIGN(len));

    return 0;
}


typedef struct {
    char family;
    char bitlen;
    unsigned char data[sizeof(struct in6_addr)];
} _inet_addr;

typedef struct {
    int iface_id;
    char *iface_name;
    int netlink_flags;
    int counter;
    char **blocked;
    
} Config;

int read_addr(char *addr, _inet_addr *res)
{
    if (strchr(addr, ':')) {
        res->family = AF_INET6;
        res->bitlen = 128;
    } else {
        res->family = AF_INET;
        res->bitlen = 32;
    }

    return inet_pton(res->family, addr, res->data);
}

int do_route(int sock, int flags, _inet_addr *dst, int if_idx){

    struct {
        struct nlmsghdr n;
        struct rtmsg r;
        char buf[4096];
    } nl_request;

    nl_request.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    nl_request.n.nlmsg_flags = NLM_F_REQUEST | flags;
    nl_request.n.nlmsg_type = RTM_NEWROUTE;
    nl_request.r.rtm_family = dst->family;
    nl_request.r.rtm_table = RT_TABLE_MAIN;
    nl_request.r.rtm_scope = RT_SCOPE_NOWHERE;
    
    nl_request.r.rtm_protocol = RTPROT_BOOT;
    nl_request.r.rtm_type = RTN_UNICAST;

    nl_request.r.rtm_dst_len = dst->bitlen;
    nl_request.r.rtm_scope = RT_SCOPE_LINK;

    /* Set destination network */
    rtattr_add(&nl_request.n, sizeof(nl_request), RTA_DST, &dst->data, dst->bitlen/8);

    /* Set interface */
    rtattr_add(&nl_request.n, sizeof(nl_request), RTA_OIF, &if_idx, sizeof(int));

    return send(sock, &nl_request, sizeof(nl_request),0);

}

int is_new(Config *conf,char *srcaddr){
    for (int i = 0 ; i < conf->counter; i++) {
        if (conf->counter > 0) {
            if (strcmp(conf->blocked[i],srcaddr) == 0) {
                return -1;
            }        
        }
    }
    return 0;
}

struct sockaddr_in source,dest;

void pkt_hdr(u_char *args,const struct pcap_pkthdr *hdr,const u_char *pkt){
    int size = hdr->len;
	struct iphdr *iph = (struct iphdr*)(pkt + sizeof(struct ethhdr));
    int iphlen = iph->ihl*4;

    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;

    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;    

    if (iph->protocol == IPPROTO_TCP) {
        struct iphdr *iph = (struct iphdr *)(pkt + sizeof(struct ethhdr));
        int iphdrlen = iph->ihl*4;

        struct tcphdr *tcph = (struct tcphdr *)(pkt + iphdrlen +sizeof(struct ethhdr));
        int header_size = sizeof(struct ethhdr)+iphdrlen+tcph->doff*4;
        if (ntohs(tcph->source) == 443 || ntohs(tcph->dest)== 443)  {
            int pktflag = (int)tcph->th_flags;
            if (pktflag == 20) {
                Config *conf = (Config *) args;
                if (is_new(conf,inet_ntoa(source.sin_addr)) == -1) {
                    return;
                }

                conf->blocked[conf->counter] = malloc((conf->counter) * sizeof(char));
                strcpy(conf->blocked[conf->counter], inet_ntoa(source.sin_addr));
                conf->counter++;

                printf("IP: %s:%d blocked by kominfo\n",inet_ntoa(source.sin_addr),ntohs(tcph->source));
                printf("Reroute %s to %s\n",inet_ntoa(source.sin_addr),conf->iface_name);

                unsigned char data[sizeof(struct in6_addr)];
                                
                int net_sock = open_netlink();
                if (net_sock < 0) {
                    exit(-1);
                }

                _inet_addr res = {0};
                if (read_addr(inet_ntoa(source.sin_addr), &res) != 1) {
                    fprintf(stderr, "Failed to parse destination network\n");
                    exit(-1);
                }

                int route_res = do_route(net_sock,conf->netlink_flags,&res,conf->iface_id);
                if (route_res == -1) {
                    fprintf(stderr, "Netlink error: %d\n", route_res);
                    exit(-1);
                }

                close(net_sock);      
            }
        }
    }
    return;
}

#define NEXT_CMD_ARG() do { argv++; if (--argc <= 0) exit(-1); } while(0)

int main(int argc,char *argv[]){
    char *dev_name,*vpn_name;
    int * blkd_arry = calloc(1, sizeof(int));
    while (argc > 0) {
        if (strcmp(*argv, "-dev") == 0) {
            NEXT_CMD_ARG();
            dev_name = *argv;
        } else if (strcmp(*argv, "-vpn") == 0) {
            NEXT_CMD_ARG();
            vpn_name = *argv;
        }

        argc--; argv++;
    }
    pcap_if_t *dev = NULL;
    char err_buff[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    int snap_len = 1024;
    u_char *my_arg = NULL;
    
    if(pcap_findalldevs(&dev, err_buff) == -1){
        fprintf(stderr, "There is a problem with pcap_findalldevs: %s\n", err_buff);
        return -1;        
    }

    while (dev != NULL) {
        if (strcmp(dev->name, dev_name) == 0) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)dev->addresses->addr;
            dev_name = dev->name;
            break;
        }
        
        dev = dev->next;
    }
    
    Config routeconf = {
        .iface_id = if_nametoindex(vpn_name),
        .iface_name = vpn_name,
        .netlink_flags = NLM_F_CREATE,
        .counter = 0,
        //TODO made blocked into global var,so when SIGINT handler all route should deleted
        .blocked = calloc(0, sizeof(int)),
    }; 

    if (routeconf.blocked == NULL) {
        printf("Unable to allocate memory\n");
        return -1;        
    }

    handle = pcap_open_live(dev_name, snap_len, 0, 1000, err_buff);
	if (handle == NULL) {
		fprintf(stderr, "Couldn't open device %s : %s\n" , dev_name , err_buff);
		exit(1);
	}    

    pcap_loop(handle, 0, pkt_hdr, (u_char *)&routeconf);
    pcap_freealldevs(dev);
    return 0;
}