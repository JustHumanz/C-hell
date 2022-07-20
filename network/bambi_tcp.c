

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/ucontext.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT "2525"
#define BACKLOG 20

char *banner =  "                                             ......\n"
                "                                    ...------mmmmmmm-----....\n"
                "                                  .--mmmmmmmmmmmmmmmmmmmmmmmm-\n"
                "                              ..--mmmmmm------......-----mmmmmm--\n"
                "                           .--mmmmm--...                 ...--mmmm-...\n"
                "              ...----------mmmmm-..                           ..-mmmmm-.\n"
                "          ...-mmmmmmmmmmmmmmm-..                                 .-mmmm-.\n"
                "         .-mmmmmm--.....----.     .                                -mmmmm.\n"
                "         .mmmmmmm--....         .-m-.         ....-----------..     ..mmmm.\n"
                "         .-mmmmmmmmmmmm--.     .-mmm-     ..--mmmmmmmmmmmmmmmmm--.    .-mmm.\n"
                "           .-mmmm--...---.       .-.   .--mmmmm----........---mmmm-.   .-mmm.\n"
                "             .-mmmm---...            .-mmmmm-..               .-mmmm-.  .-mmm-.\n"
                "               ..-mmmmmmm-.         .mmmm-.                     .-mmmm.  .-mmmm.\n"
                "                  .-mmmmm-.        .-mmm-.                       -mmmm.   .mmmmm.\n"
                "    ..---.         -mmmm-.         .-mm-..                       -mmmm.    .mmmm-.\n"
                "   .mmmmmm---.    -mmmm-           .-mmm-..                   ..-mmmm-      .-mmm-.\n"
                " .-mmmm-mmmmmm-. .mmmm-.            .-mmmmm----.........------mmmm-..         .mmmm.\n"
                ".-mmmm-..--mmmmm-mmmmm-. ...-------.  .---mmmmmmmmmmmmmmmmmmmmm--.             .-mmm.\n"
                ".mmmm-......-mmmmmmmmmmm-mmmmmmmmmmm.     ..---mmmmmmmm------..                 .mmmm.\n"
                ".mmmm-......--mmm---mmmmmmmmmmmmmmmm-         ..........                         .mmmm..\n"
                ".mmmmm----mmmmmmm---mmmmm----...-mmm-                                             .-mmmm-.\n"
                ".-mmmmmmmmmmmmmmmmmmmmmmm--.....-mmm-                                               .mmmmm-..        ..\n"
                " ...---mmmmmm--mmmmmmmmmmmm--..-mmmm.                                                .--mmmmm--------mm-\n"
                "    .-mmmm----mmmmmm--mmmmmmmm-mmmm.                                                    ..--mmmmmmmmmm--.\n"
                "   .-mmmm-..-mmmmmmm-.-mmmmmmmmmmm-                                                        ..-------...--.\n"
                " .-mmmmm-.---mmmmmmm-.-mmmm---mm--.                                                              ...--mmm--.\n"
                " .--mmmmm-mmmmm-mmmm...-mmm.  ..                                                               .--mmmmm---...\n"
                "    .--mmmmmmm-.-mmm----mmm-.                                                                  ..-m------mmmm.\n"
                "       ..----. .-mmmmmmmmmmm.                                                                     .--mmmmmm--.\n"
                "                ---mmmmmm---.                                                                    .-mmmmm-...\n"
                "                  ........                                                                     ..-mmm-..\n"
                "                  .-mm-.                                                                     .-mmmmm-\n"
                "                  .mmmmm-.                                                                 .-mmmmm-.\n"
                "                   .-mmmm-..                                                             .-mmmm--.\n"
                "                     ..-mmmm-..                                                     ..---mmmm-.\n"
                "                        -mmmmmm-..                                                .--mmmmmm-.\n"
                "                         .--mmmmm---..                                      ....--mmmmmm--.\n"
                "                            .---mmmmmm-----....                   ......----mmmmmmm--...\n"
                "                                ...--mmmmmmmmmmm------------------mmmmmmmmmmm----..\n"
                "                                     .....-----mmmmmmmmmmmmmmmmmmmm------...\n"
                "                                              ...........-.........\n"
                "~JustHumanz~\n";
                
void sigchl_handler(int pid){
    int saved_error= errno;

    while (waitpid(-1,NULL,WNOHANG) > 0) {
        errno = saved_error;
    }
}

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void){
    int sockfd,new_fd;
    struct addrinfo hints,*servinfo,*p;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n",gai_strerror(rv));
        return 1;
    }

    while (servinfo != NULL) {
    if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1){
        perror("server: socket");
        continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
        perror("server: socket");
        exit(1);
    }

    if (bind(sockfd,servinfo->ai_addr,servinfo->ai_addrlen) == -1) {
        close(sockfd);
        perror("server: bind");
        continue;
    }

    servinfo = servinfo->ai_next;
    break;

    }

    freeaddrinfo(servinfo);

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchl_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("Server listening");

    while (1) {
        sin_size = sizeof(their_addr);
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("acc");
            exit(1);
        }

        inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);

        printf("Server: got connection from %s\n",s);

        if(!fork()){
            close(sockfd);
            if (send(new_fd,banner,strlen(banner),0) == -1) {
                perror("send");
            }
            close(new_fd);
            exit(0);
        }
        close(new_fd);
    }



    return 0;
}