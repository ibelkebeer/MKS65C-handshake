#include "pipe_networking.h"

static void sighandler(int signo){
  if(signo == 2){
    char name[10];
    sprintf(name, "%d", getpid());
    unlink(name);
    exit(1);
  }
}

int main() {

  signal(SIGINT, sighandler);

  int to_server;
  int from_server;

  from_server = client_handshake( &to_server );
}
