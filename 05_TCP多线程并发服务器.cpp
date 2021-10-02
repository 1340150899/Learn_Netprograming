#include<iostream>
#include<cstring>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<pthread.h>
using namespace std;

void *deal_clien_fun(void* arg){
    int fd = *(int*) arg;
    while(1){
	char buf[128] = "";
	int len = recv(fd, buf, sizeof(buf), 0);

	if(len == 0) break;
	send(fd, buf, len, 0);
    }

    cout<< "close"<<endl;

    close(fd);
    return nullptr;
}

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
	perror("socket");
    }
    int yes = 1;

    //SO_REUSEADDR
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in hostaddr;
    hostaddr.sin_family = AF_INET;
    hostaddr.sin_port = htons(9999);
    hostaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    int bindres = bind(sockfd, (struct sockaddr*)& hostaddr, sizeof(hostaddr));
    if(bindres == -1){
	perror("bindres");
    }

    listen(sockfd, 10);

    while(true){
	struct sockaddr_in clien;
	socklen_t len = sizeof(clien);
	int sock_clien = accept(sockfd, (struct sockaddr*)&clien, &len);
	char ip[16] = "";
	inet_ntop(AF_INET, &clien.sin_addr.s_addr, ip, 16);
	cout<< ip<< " : "<< clien.sin_port<< " connect!"<< endl;
	
	pthread_t tid;
	pthread_create(&tid, nullptr, deal_clien_fun, (void*)&sock_clien);
	pthread_detach(tid);
    }
    
    close(sockfd);
    return 0;

}
