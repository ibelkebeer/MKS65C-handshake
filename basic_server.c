#include "pipe_networking.h"

static void sighandler(int signo){
  if(signo == 2){
    unlink(ACK);
    char newname[10];
    sprintf(newname, "%d", getpid());
    unlink(newname);
    exit(0);
  }
}

int main() {

  signal(SIGINT, sighandler);

  int to_client;
  int from_client;

  from_client = server_handshake( &to_client );
}
