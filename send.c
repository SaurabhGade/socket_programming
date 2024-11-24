#include "./std_headers.h"
int main(int argc, char **argv){
  if(argc != 2)
    err("missing argument\n./send <port>");
  port_t port;
  sscanf(argv[1], "%hu", &port);
  send_message(port, stdin);
  return 0;
}
