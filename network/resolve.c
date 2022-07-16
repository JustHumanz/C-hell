#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char * argv[]){
    struct addrinfo hints,*res,*p;
    int status;

    char ipstr[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"invalid args");
        return -1;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(argv[1], NULL, &hints, &res))!= 0) {
        fprintf(stderr, "getaddrinfo %s\n",gai_strerror(status));
        return 2;
    }

    printf("IP address %s:\n",argv[1]);

    while (res != NULL) {
        void *addr;
        char *ipver;

        if (res->ai_family == AF_INET){
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)res->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        inet_ntop(res->ai_family,addr,ipstr,sizeof(ipstr));
        printf("%s %s\n",ipver,ipstr);
        res = res->ai_next;
    }

    freeaddrinfo(res);

    return 0;
}