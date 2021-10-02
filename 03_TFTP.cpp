#include<iostream>
#include<cstdio>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<cstring>
#include<sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>
using namespace std;
int main(int argc, char* argv[])
{
    if(argc != 3){
	
	cout<< "?"<< endl;
    }
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
	perror("socket");
	return 0;
    }
    char cmd[128] = "";
    //按照tftp协议的格式组请求报文     octet 二进制模式  netascii 文本模式
    int len = sprintf(cmd, "%c%c%s%c%s%c",0x00, 0x01, argv[2], 0, "netascii", 0);
    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(69);
    inet_pton(AF_INET, argv[1], &server.sin_addr.s_addr);
    sendto(sockfd, cmd, len, 0, (struct sockaddr*)& server, sizeof(server));
    //open
    int fd = open(argv[2], O_WRONLY|O_CREAT,0666);
    if(fd < 0){
	perror("open");
	return 0;
    }
    unsigned short num = 0;
    while(1){
	unsigned char buf[1024] = "";
	struct sockaddr_in from;
	socklen_t from_len = sizeof(from);
	int len = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&from, &from_len);
	if(buf[1] == 0x03){
	    //num用来检测发来的报文是否重复 协议的第三个字节开始时快编号
	    //只有当快编号正确时才收数据因为是网络字节序所以需要转换
	    if(num+1 == ntohs(*(unsigned short*)(buf+2))){
		write(fd, buf+4, len-4);
		num = ntohs(*(unsigned short*)(buf+2));
		cout<< "recv: "<< num<< endl;
	    }
	    buf[1] = 4;
	    sendto(sockfd, buf, 4, 0, (struct sockaddr*)& from, sizeof(from));
		if(len < 516) break;
	}
    }
    close(sockfd);
    close(fd);
    return 0;
}
