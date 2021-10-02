#include<iostream>
#include<cstring>
#include<sys/socket.h>
#include<pthread.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
using namespace std;

void deal_clien_fun(int fd){
    while(1){
	char buf[128] = "";
	
	int len = recv(fd, buf, sizeof(buf), 0);
	if(len == 0) break;

	send(fd, buf, len, 0);
    }
    return;
}

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
	perror("socket");
    }

    int yes = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in tcpsock;
    tcpsock.sin_family = AF_INET;
    tcpsock.sin_port = htons(9000);
    tcpsock.sin_addr.s_addr = htonl(INADDR_ANY);

    int bindres = bind(sockfd, (struct sockaddr*)& tcpsock, sizeof(tcpsock));
    if(bindres == -1){
	perror("bind");
    }

    listen(sockfd, 10);

    while(true){
	struct sockaddr_in clien;
	socklen_t len = sizeof(clien);
	int clienfd = accept(sockfd, (struct sockaddr*)&clien, &len);
	
	char ip[16] = "";
	inet_ntop(AF_INET, &clien.sin_addr.s_addr, ip, 16);
	cout<< ip<< " : "<< clien.sin_port<< " connect!"<< endl;

	pid_t pid;
	if(fork() == 0){
	    close(sockfd);

	    deal_clien_fun(clienfd);

	    close(clienfd);

            _exit(-1);
	}else{
	    close(clienfd);
	}
    }
    close(sockfd);
    return 0;
}
