#include <linux/netlink.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
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

int main(){
    unsigned char data[sizeof(struct in6_addr)];
    int if_idx = if_nametoindex("virbr0");
    int nl_sock = open_netlink();
    int nl_flags = NLM_F_CREATE | NLM_F_EXCL;

    if (nl_sock < 0) {
        exit(-1);
    }

    _inet_addr res = {0};
    if (read_addr("172.16.18.0", &res) != 1) {
        fprintf(stderr, "Failed to parse destination network\n");
    }

    do_route(nl_sock,nl_flags,&res,if_idx);
    close(nl_sock);
    return 0;
}