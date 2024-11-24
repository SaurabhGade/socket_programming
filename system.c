#include "./std_headers.h"
int main(int argc, char **argv){
  if(argc != 2)
    err("Missing argument\n./system <port>");
  port_t port;
  sscanf(argv[1], "%hu", &port);
  init_system(port);
  online();
  fclose(sys.sys_file);
}
