#include "./std_headers.h"
int main(int argc, char **argv){
  if(argc != 2)
    err("missing argument\n./send <port>");
  port_t port;
  sockfd_t fd;
  sockaddr_in_t addr;
  sscanf(argv[1], "%hu", &port);
  fd = getIPv4UDPSocket();
  getIPv4UDPAddress("", port, &addr);

  return 0;
}
