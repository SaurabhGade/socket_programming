#include "./std_headers.h"
#define OPT_CNT 4 
void print_all_options(){
  refresh_system();
  char *op[OPT_CNT];
  op[0] = "0. Enter 0 to quit\n";
  op[1] = "1. Print all options\n";
  op[2] = "2. Print online users\n";
  op[3] = "3. Start communication\n";
  fprintf(stdout, "%s", op[0]);
  fprintf(stdout, "%s", op[1]);
  fprintf(stdout, "%s", op[2]);
  fprintf(stdout, "%s", op[3]);
  return;
}
void print_chat_options(){
  fprintf(stdout, "%s\n", "0: To send the message"       );
  fprintf(stdout, "%s\n", "1: To print received messages");
  fprintf(stdout, "%s\n", "2: To stop current chat"      );
  fprintf(stdout, "%s\n", "3: To print all options"      );
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
  FILE *temp  = fopen(".##temp_file_##", "wr+");
  if(temp == NULL) sys_err_handler("Cannot open the file\n");  
  size_t t;
  print_chat_options();
  pid_t pid;
  char curr_process_port[10];
  char  cli_process_port[10];
  sprintf(curr_process_port, "%hu", sys.currnt_system_port);
  sprintf(cli_process_port, "%hu", port);
  getchar();    //remove \n from stdout buff. 
  bool flag = true;
  while(flag){
    scanf("%zu", &t);
    getchar();
    switch(t){
      case 0:
        if((pid = fork()) < 0) sys_err_handler("Cannot create file");
        if(pid == 0){
          execlp("./send", "send", cli_process_port, NULL);
        }
        else wait(NULL); 
        break;
      case 1:
        if((pid = fork()) < 0) sys_err_handler("Cannot create file");
        if(pid == 0){
          execlp("./receive", "receive", curr_process_port, NULL);
        }
        else wait(NULL);
        break;
      case 2:
        flag = false;
        break;
      case 3:
        print_chat_options();
        break;
      default:
        fprintf(stdout, "%s\n", "Choose the correct option");
        break;
    }
  }
  return;
}
void start_communication(){
  char t;
  print_all_options();
  bool flag = true;
  while(flag){
    bzero(&t, sizeof(t));
    scanf("%c", &t);
    getchar();
    switch(t){
      case '0':
        flag = false;
        break;
      case '1':
        print_all_options();
        break;
      case '2':
        print_online_users();
        break;
      case '3':
        size_t  id;
        fprintf(stdout, "%s", "Enter system id\n");
        fscanf(stdin, "%zu", &id);
        if(!validate_sys(id)) fprintf(stdout, "%s\n", "Not a valid system id");
        start_chat(id);
        break;
      case '4':
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
  offline();
  fclose(sys.sys_file);
}
