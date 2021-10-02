#include<iostream>
#include<sys/socket.h>
#include<cstring>
#include<unistd.h>
#include<arpa/inet.h>
using namespace std;
int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
	perror("TCPsocket");
	return 0;
    }
    cout<< "socket:"<< sockfd<< endl;
    char ip[]= "192.168.65.128";
    struct sockaddr_in tcpaddr;
    bzero(&tcpaddr, sizeof(tcpaddr));
    tcpaddr.sin_family = AF_INET;
    tcpaddr.sin_port = htons(9000);
    inet_pton(AF_INET, ip, &tcpaddr.sin_addr.s_addr);
    bind(sockfd, (struct sockaddr*)&tcpaddr, sizeof(tcpaddr));

    struct sockaddr_in tcp1;
    tcp1.sin_family = AF_INET;
    tcp1.sin_port = htons(9000);
    tcp1.sin_addr.s_addr = inet_addr("192.168.1.103");

    connect(sockfd, (struct sockaddr*)&tcp1, sizeof(tcp1));

    //发送
    char buf[128] = "";
    cout<< "发送：";
    cin >> buf;
    //buf[strlen(buf)-1] = 0;
    send(sockfd, buf, strlen(buf), 0);

    //接收
    char str[128] = "";
    recv(sockfd, str, sizeof(str), 0);
    cout<< "应答："<< str<< endl;

    close(sockfd);
    return 0;
}
