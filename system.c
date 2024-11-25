#include "./std_headers.h"
#define OPT_CNT 5 
void print_all_options(){
  char *op[OPT_CNT];
  op[0] = "Eter q to quit\n";
  op[1] = "1. Print all options\n";
  op[2] = "2. Print online users\n";
  op[3] = "3. Start communication\n";
  op[4] = "5. Exit\n"; 
}
bool validate_sys(size_t id){
  if(sys.online_count <= id)
    return false;
  if(sys.users_port[id] == 0)
    return false;
}
void start_communication(){
  unsigned char t;
  print_all_options();
  bool flag = true;
  while(flag){
    sscanf(stdin, "%hhu", &t);
    switch(t){
      case 'q':
        flag = false;
        break;
      case 1:
        print_all_options();
        break;
      case 2:
        print_online_users();
        break;
      case 3:
        size_t  id;
        fprintf(stdout, "%s", "Enter system id\n");
        sscanf(stdin, "%zu", &id);
        start_chat(id);
        break;
    }
  }
}
int main(int argc, char **argv){
  if(argc != 2)
    err("Missing argument\n./system <port>");
  port_t port;
  sscanf(argv[1], "%hu", &port);
  init_system(port);
  online();
  print_online_users();
  offline();
  fclose(sys.sys_file);
}
