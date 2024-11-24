#include "./std_headers.h"
int main(int argc, char **argv){
  if(argc != 2)
    err("Missing argument\n./receive <port>");
  port_t port;
  sscanf(argv[1], "%hu", &port);
  receive_message(port, stdout);
  return 0;
}
