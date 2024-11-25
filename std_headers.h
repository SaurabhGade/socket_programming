#ifndef __STD_HEADERS__
#define __STD_HEADERS__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#include <assert.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define BUFF_SIZE     1024
#define MAX_SYSTEMS   128 
char buff[BUFF_SIZE];


/* communication related methods. */
typedef struct sockaddr_in     sockaddr_in_t;
typedef struct sockaddr        sockaddr_t;  
typedef unsigned short         port_t;
typedef int                    sockfd_t;


void    err                   (char *msg);
int     getIPv4UDPSocket      ();
void    getIPv4UDPAddress     (char *ip, port_t port, sockaddr_in_t *addr);
void    bindIPv4UDPAddress    (sockfd_t fd, sockaddr_in_t *addr);
size_t  send_message          (port_t port, FILE *input);
size_t  receive_message       (port_t port, FILE *output);





/*  system related methods */
typedef struct usrt_port_info{
  port_t currnt_system_port;
  port_t users_port[MAX_SYSTEMS];
  size_t online_count;
  FILE  *sys_file;
  bool  isOnline;
} upi_t;
upi_t sys = {.currnt_system_port = 0, .online_count = 0, .sys_file = NULL}; 

void init_system(port_t port);
void sys_err_handler    (char *msg);
bool is_port_available  (); 
void refresh_system     ();
void online             ();
void offline            ();
void print_online_users ();


#endif // __STD_HEADER__

#ifndef __STD_HEADER__IMPL
#define __STD_HEADER__IMPL

/* communication methods definations */
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
size_t send_message(port_t port, FILE *input){
  sockfd_t fd;
  sockaddr_in_t addr;
  fd = getIPv4UDPSocket();
  getIPv4UDPAddress("", port, &addr);

  size_t count = 0;
  size_t n, cnt, sbytes;
  char *msg = NULL;
  while(true){
    n = getline(&msg, &cnt, input);
    sbytes = sendto(fd, msg, n, 0, (sockaddr_t*)&addr, sizeof(addr));
    if(strcmp(msg, "Exit\n") == 0)
      break;
    count += sbytes;
    free(msg);
  }
  close(fd);
  return count;
}

size_t receive_message(port_t port, FILE *output){
  sockfd_t fd;
  sockaddr_in_t cli, serv;
  socklen_t addrLen;
  size_t n;
  size_t count = 0;
  memset(&serv, 0, sizeof(serv));
  fd = getIPv4UDPSocket();
  getIPv4UDPAddress("", port, &serv);
  bindIPv4UDPAddress(fd, &serv);
  while(true){
    n = recvfrom(fd, buff, BUFF_SIZE-1, 0, (sockaddr_t*)&cli, &addrLen);
    buff[n] = 0;
    if(strcmp(buff, "Exit\n") == 0)
      break;
    fprintf(output, "%s", buff);
    count += n;
  }
  close(fd);
  return count;
}

/* System methods definations */
/*
typedef struct usrt_port_info{
  port_t currnt_system_port;
  port_t users_port[100];
  size_t online_count;
  FILE  *sys_file;
} upi_t;
upi_t sys_head = {.currnt_system_port = 0, online_count = 0, .sys_file = NULL}; 
void refresh_system     ();
void online             ();
void offline            ();

*/
void init_system(port_t port){
  assert(port != 0);
  sys.currnt_system_port = port;
  sys.online_count       = 0;
  sys.isOnline           = false;
  memset(&sys.users_port, 0, MAX_SYSTEMS);
  sys.sys_file = fopen("./.sysinfo", "rb+");
  if(sys.sys_file == NULL){
    sys.sys_file = fopen("./.sysinfo", "wb+");
    if (sys.sys_file == NULL){
      err("Cannot open sysinfo");
    }
  }
}

void sys_err_handler    (char *msg){
  if(sys.sys_file != NULL)
    fclose(sys.sys_file);
  err(msg);
}
void refresh_system     (){
  assert(sys.sys_file != NULL);
  sys.online_count = 0;
  memset(sys.users_port, 0 , MAX_SYSTEMS*sizeof(port_t)); 
  rewind(sys.sys_file);
  port_t t;
  while(fread(&t, sizeof(t), 1, sys.sys_file) == 1){
    if(t != 0 && t != sys.currnt_system_port)
      sys.users_port[sys.online_count++] = t;
  }
}
bool is_port_available(){
  rewind(sys.sys_file);
  port_t t;
  while(fread(&t, sizeof(port_t), 1, sys.sys_file) == 1)
    if(t == sys.currnt_system_port)
      return false;
  return true;
}
void online(){
  assert(sys.sys_file != NULL);
  refresh_system();
  if(!is_port_available())
    sys_err_handler("Port not available\n");
  rewind(sys.sys_file);
  port_t t;
  bool flag = false;
  long pos;
  while(fread(&t, sizeof(port_t), 1, sys.sys_file) == 1)
    if(t == 0) break;
  if(feof(sys.sys_file)){   // zero not found
    fwrite(&sys.currnt_system_port, sizeof(port_t), 1, sys.sys_file);
  } else { // zero found 
    pos = ftell(sys.sys_file);
    if(pos > 0)
      fseek(sys.sys_file, pos-sizeof(port_t), SEEK_SET);
    fwrite(&sys.currnt_system_port, sizeof(port_t), 1, sys.sys_file);
  }
  sys.isOnline = true;
}
void offline(){
  assert(sys.sys_file != NULL);
  rewind(sys.sys_file);
  long pos;
  port_t t;
  while(fread(&t, sizeof(port_t), 1, sys.sys_file) == 1){
    if(t == sys.currnt_system_port){
      pos = ftell(sys.sys_file);
      if(pos > 0){
        fseek(sys.sys_file, pos - sizeof(port_t), SEEK_SET);
        t = 0;
        fwrite(&t, sizeof(port_t), 1, sys.sys_file);
        fflush(sys.sys_file);
      }
      break;
    }
  }
  sys_err_handler("System is offline\n");
}

void print_online_users (){ 
  refresh_system();
  fprintf(stdout, "%s %zu %s", "Online:\t", sys.online_count, "\n");
  for(int i = 0; i < sys.online_count; i++){
    if(sys.users_port[i])
      fprintf(stdout, "id = %d, port = %hu\n", i, sys.users_port[i]);
  }
}
#endif /* __STD_HEADER__IMPL */
