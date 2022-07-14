#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_BUF_LEN 1024
#define MAX_METHOD_LEN 10
#define MAX_PATH_LEN 800
#define MAX_HTTPSTR_LEN 12

int parse_http(const char* http_req,const int req){
    char method[MAX_METHOD_LEN];
    char path[MAX_PATH_LEN];
    char http[MAX_HTTPSTR_LEN];

    int i,j =0;

    if (http_req == NULL){
        return -1;
    }

    printf("REQUEST-LEN: %d\n",req);

    while (http_req[i] != ' ') {
        if(http_req[i] == NULL){
            return -2;
        }
        
        if (j >= MAX_METHOD_LEN-1){
            return -3;
        }

        if (i == req){
            return -4;
        }

        method[j++] = http_req[i++];
    }    

    method[j] = '\0';

    printf("METHOD: %s\n",method);

    if (strcmp(method, "GET") != 0){
        return -11;
    }

    j = 0;
    i++;

    while (http_req[i] != ' ' && (http_req[i] != NULL)) {
        if (j >= MAX_HTTPSTR_LEN-1){
            return -7;
        }

        if (i == req){
            return -8;
        }

        http[j++] = http_req[i++];
    }

    http[j] = '\0';

    return 1;
}

int handle_client(int connfd) {
    char buffer[MAX_BUF_LEN+1];
    int n = 0;
    int req_type = 0;
    const char send_ok[1024] = "HTTP/1.0 200 OK\r\nserver: Humanz\r\ncontent-type: text/html; charset=UTF-8\r\n\n";

    n = recv(connfd, buffer, MAX_BUF_LEN, 0);

    if (n <= 0){
        perror("recv() errr");
        close(connfd);
    }

    buffer[n] = '\0';

    printf("HTTP-REQUEST: %s\n",buffer);

    req_type = parse_http(buffer, n);

    if (req_type == 0 ){
        printf("Simple req\n");
        send(connfd,send_ok,strlen(send_ok),0);
    } else if (req_type == 1) {
        printf("multiple req\n");
        send(connfd,send_ok,strlen(send_ok),0);
    } else {
        fprintf(stderr, "parse error: %d\n",req_type);
        close(connfd);
        return -1;
    }

    char sendmsg[1024] = "<html><head><title>Humanz</title></head><body>From C-world,hello humanz</body></html>";
    if(send(connfd,sendmsg,strlen(sendmsg),0) <0 ){
        perror("send error");
    }

    close(connfd);

    return 0;
}

#include <sys/wait.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>

unsigned int clientcount;

void sigchld_handler(int sig){
    int status;
    pid_t pid;

    while ((pid = waitpid(-1,&status,WNOHANG)) > 0) {
        clientcount--;
        printf("%i exited with %i\n", pid, WEXITSTATUS(status));
    }

    return;
}

int main(int argc,char* argv[]){
    int sockfd,connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    struct sigaction sa;

    clientcount = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0 ){
        perror("socket err");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(2525);

    if(bind(sockfd, (struct sockaddr*)&servaddr,sizeof(servaddr)) != 0) {
        perror("bind error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if(listen(sockfd, 2525) < 0) {
        perror("listen error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = sigchld_handler;
    sigaction(SIGCHLD,&sa,NULL);

    while (1) {
        if (clientcount < 10) {
            clilen = sizeof(cliaddr);

            connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
            if (connfd < 0) {
                if (errno == EINTR) {
                    continue;
                }

                perror("accept error");
                close(sockfd);
                exit(EXIT_FAILURE);
            }

            clientcount++;

            childpid = fork();
            if (childpid < 0){
                perror("fork error");
                exit(EXIT_FAILURE);
            }

            if (childpid == 0 ){
                close(sockfd);
                exit(handle_client(connfd));
            }

            printf("Client PID %d",childpid);

            close(connfd);            
        }
    }

    return EXIT_SUCCESS;

}
