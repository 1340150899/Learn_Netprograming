#include<iostream>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<cstring>
using namespace std;
int main()
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in myaddr;
    //bzero头文件 cstring
    bzero(&myaddr, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(9000);
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(sockfd, (struct sockaddr*)&myaddr, sizeof(myaddr));

    //加入多播组
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr("224.0.0.2");
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));

    while(1){
	unsigned char buf[1024] = "";
	recvfrom(sockfd, buf, sizeof(buf), 0, nullptr, nullptr);
	cout<< "多播："<< buf<< endl;
    }
    close(sockfd);
    return 0;
}
