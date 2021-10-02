#include<iostream>
#include<cstring>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
using namespace std;

char head[] = "HTTP/1.1 200 OK\r\n"		    \
	      "Content-Type: text/html\r\n"	    \
	      "\r\n";
char err[]= "HTTP/1.1 404 Not Found\r\n"	    \
	    "Content-Type: text/html\r\n"	    \
	    "\r\n"				    \
	    "<HTML><BODY>File not found</BODY></HTML>";

void* deal_clien_fun(void *arg){
    int sockfd = *(int* )arg;
    unsigned char buf[512] = "";
    recv(sockfd, buf, sizeof(buf), 0);

    char file_name[128] = "./html/";
    sscanf((const char *)buf, "GET /%[^ ]", file_name + 7);
    if(file_name[7] == '\0'){
	strcpy(file_name, "./html/index.html");
    }

    int fd = open(file_name, O_RDONLY);
    if(fd < 0){
	perror("open");
	send(sockfd, err, strlen(err), 0);
    }else{
	send(sockfd, head, strlen(head), 0);
	while(1){
	    unsigned char buf[1024] = "";
	    int ret = read(fd, buf, sizeof(buf));
	    cout<< "ret = "<< ret<< endl;
	    if(ret < 1024){
		send(sockfd, buf, ret, 0);
		break;
	    }
	    send(sockfd, buf, ret, 0);
	}
    }
    close(fd);
    return nullptr;
}

int main(int argc, char *argv[])
{
    if(argc != 2){
	cout<< "please input port!"<< endl;
	return 0;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
	perror("socket");
	return 0;
    }
    
    int yes = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in tcpaddr;
    tcpaddr.sin_family = AF_INET;
    tcpaddr.sin_port = htons(atoi(argv[1]));
    tcpaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    int bindres = bind(sockfd, (struct sockaddr*)&tcpaddr, sizeof(tcpaddr));
    if(bindres == -1){
	perror("bind");
	return 0;
    }

    listen(sockfd, 10);

    while(1){
	struct sockaddr_in clien;
	socklen_t len = sizeof(clien);
	int clien_fd = accept(sockfd, (struct sockaddr*)& clien, &len);

	char ip[16] = "";
	inet_ntop(AF_INET, &clien.sin_addr.s_addr, ip, 16);
	cout<< ip<< " : "<< clien.sin_port<< " connect!"<< endl;

	pthread_t tid;
	pthread_create(&tid, nullptr, deal_clien_fun, (void *)&clien_fd);
	pthread_detach(tid);

    }

    close(sockfd);
    
    return 0;
}
