#include<iostream>
#include<cstdio>
#include<cstring>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
using namespace std;
void *my_send_fun(void *arg){
    int sockfd = *(int *)arg;
    
    //绑定套接字和udp结构体
    struct sockaddr_in dst_addr;
    bzero(&dst_addr, sizeof(dst_addr));
    dst_addr.sin_family = AF_INET;

    //循环发送
    while(1){
	char buf[128] = "";
	fgets(buf, sizeof(buf), stdin);
	buf[strlen(buf)-1] = 0;
	if(strncmp(buf, "sayto", 5) == 0){
	    unsigned short port = 0;
	    char ip[16] = "";
	    sscanf(buf, "sayto %s %hd", ip, &port);
	    dst_addr.sin_port = htons(port);
	    inet_pton(AF_INET, ip, &dst_addr.sin_addr.s_addr);
	    continue;
	}else{
	    sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&dst_addr, sizeof(dst_addr));
	}
    }
}

int main(int argc, char *argv[])
{
    //如果没有指定端口就错误
    if(argc != 2){
	cout<< "please input port:"<< endl;
    }
    //创建套接字 失败直接返回
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
	perror("socket");
	return 0;
    }	
    //新建udp结构体绑定套接字和结构体
    struct sockaddr_in myaddr;
    bzero(&myaddr, sizeof(myaddr));

    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(atoi(argv[1]));

    bind(sockfd, (struct sockaddr*)&myaddr, sizeof(myaddr));

    //创建一个新线程 发消息
    pthread_t tid;
    pthread_create(&tid, nullptr, my_send_fun, (void*)&sockfd);
    pthread_detach(tid);
    

    //收消息
    while(1){
	char buf[128] = "";
	struct sockaddr_in from;
	socklen_t len = sizeof(from);
	recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&from, &len);
	unsigned short port = ntohs(from.sin_port);
	char ip[16]="";
	inet_ntop(AF_INET, &from.sin_addr.s_addr, ip, 16);
	cout<< "来自："<< ip<< ":"<< port<< endl<< buf<< endl; 
    }
    return 0;
}
