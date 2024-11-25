#include "./std_headers.h"
#define OPT_CNT 5 
void print_all_options(){
  char *op[OPT_CNT];
  op[0] = "Eter q to quit\n";
  op[1] = "1. Print all options\n";
  op[2] = "2. Print online users\n";
  op[3] = "3. Start communication\n";
  op[4] = "4. Exit\n"; 
  fprintf(stdout, "%s", op[0]);
  fprintf(stdout, "%s", op[1]);
  fprintf(stdout, "%s", op[2]);
  fprintf(stdout, "%s", op[3]);
  fprintf(stdout, "%s", op[4]);
  return;
}
bool validate_sys(size_t id){
  refresh_system();
  if(sys.online_count <= id)
    return false;
  if(sys.users_port[id] == 0)
    return false;
  return true;
}
void start_chat(size_t id){
  port_t port = sys.users_port[id];
  return;
}
void start_communication(){
  unsigned char t;
  print_all_options();
  bool flag = true;
  while(flag){
    fscanf(stdin, "%hhu", &t);
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
        fscanf(stdin, "%zu", &id);
        if(!validate_sys(id)) fprintf(stdout, "%s\n", "Not a valid system id");
        start_chat(id);
        break;
      case 4:
        flag = false;
        break;
      default:
        fprintf(stdout, "%s\n", "Choose the correct option");
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
  start_communication();
//  offline();
  fclose(sys.sys_file);
}
