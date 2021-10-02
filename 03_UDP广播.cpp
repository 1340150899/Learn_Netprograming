#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<cstring>
#include<arpa/inet.h>
#include<unistd.h>
using namespace std;
int main()
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    int yes = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes));

    struct sockaddr_in dst_addr;
    dst_addr.sin_family = AF_INET;
    dst_addr.sin_port = htons(9000);
    dst_addr.sin_addr.s_addr = inet_addr("192.168.65.255");
    char msg[]= "Hello I am broadcast";
    sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&dst_addr, sizeof(dst_addr));
    //close头文件#include<unistd>
    close(sockfd);
    return 0;
}
