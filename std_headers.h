#ifndef __STD_HEADERS__
#define __STD_HEADERS__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include <assert.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define BUFF_SIZE 1024
typedef struct sockaddr_in     sockaddr_in_t;
typedef struct sockaddr        sockaddr_t;  
typedef unsigned short         port_t;
typedef int                    sockfd_t;
void err(char *msg);
int getIPv4UDPSocket();
void getIPv4UDPAddress(char *ip, port_t port, sockaddr_in_t *addr);
void bindIPv4UDPAddress(sockfd_t fd, sockaddr_in_t *addr);

#endif // __STD_HEADER__

#ifndef __STD_HEADER__IMPL
#define __STD_HEADER__IMPL
void err(char *msg){
  assert(msg != NULL);
  write(1, msg, strlen(msg));
  write(1, "\n", 1);
  exit(0);
}

int getIPv4UDPSocket(){
  sockfd_t fd;
  if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    err("Socket creation failed");
  return fd;
}
void getIPv4UDPAddress(char *ip, port_t port, sockaddr_in_t *addr){
  assert(ip != NULL);
  assert(addr != NULL);
  memset(addr, 0, sizeof(*addr));
  addr->sin_family        = AF_INET;
  addr->sin_port          = htons(port);
  if(ip == "")
    addr->sin_addr.s_addr = INADDR_ANY;
  else inet_pton(AF_INET, ip, &addr->sin_addr.s_addr);
  return;
}

void bindIPv4UDPAddress(sockfd_t fd, sockaddr_in_t *addr){
  assert(addr != NULL);
  assert(fd >= 0);
  if(bind(fd, (sockaddr_t*)addr, sizeof(*addr)) != 0)
    err("Bind failed\n");
  return;
}
#endif // __STD_HEADER__IMPL
