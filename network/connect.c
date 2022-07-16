#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>

int main(){
    struct addrinfo hints, *res;
    int sockfd;
    int addr_status;
    int connect_status;

    memset(&hints, 0, sizeof(hints));

    if ((addr_status = getaddrinfo("localhost", "2525", &hints, &res)) != 0){
        fprintf(stderr, "getaddrinfo %s\n",gai_strerror(addr_status));
        return 2;
    }

    sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);

    if ((connect_status = connect(sockfd, res->ai_addr, res->ai_addrlen)) != 0) {
        fprintf(stderr, "connect %s\n",gai_strerror(connect_status));
        return 2;
    }

    char *msg = "Humanz";
    int len,byte_sent;

    len = strlen(msg);
    byte_sent = send(sockfd,msg,len,0);

    return 0;
}