#include<iostream>
#include<cstring>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
using namespace std;
int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
	perror("socket");
	return 0;
    }
    
    struct sockaddr_in tcpaddr;
    bzero(&tcpaddr, sizeof(tcpaddr));
    tcpaddr.sin_family = AF_INET;
    tcpaddr.sin_port = htons(9000);
    tcpaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(sockfd, (struct sockaddr*)&tcpaddr, sizeof(tcpaddr));
    
    listen(sockfd, 10);

    struct sockaddr_in cli_addr;
    socklen_t len = sizeof(cli_addr);
    int new_fd = accept(sockfd, (struct sockaddr*)&cli_addr, &len);

    char ip[16] = "";
    inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, ip, 16);
    cout<< ip<<":"<< cli_addr.sin_port<<" 连接"<< endl;

    char buf[128] ="";
    recv(new_fd, buf, sizeof(buf), 0);
    cout<< "客户端请求："<< buf<< endl;

    send(new_fd, "recv", strlen("recv"), 0);

    close(new_fd);

    close(sockfd);
    return 0;
}
