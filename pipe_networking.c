#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client
  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.
  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  unlink(ACK);
  if(mkfifo(ACK, 0666) == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  printf("Server: WKP made\n");
  int fifo = open(ACK, O_RDONLY);
  char name[256];
  if(read(fifo, name, sizeof(name)) == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  printf("Server: Got private pipe name: %s\n", name);
  int upstream = fifo;
  close(fifo);
  fifo = open(name, O_WRONLY);
  if(fifo == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  printf("Server: Connected to private pipe\n");
  *to_client = fifo;
  if(write(fifo, "I gotchu", strlen("I gotchu")) == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  close(fifo);
  fifo = open(ACK, O_RDONLY);
  char message[256];
  if(read(fifo, message, sizeof(message)) == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  close(fifo);
  printf("Server: Got message from client: %s\n", message);
  if(unlink(ACK) == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  printf("Server: Deleted WPK\n");
  return upstream;
}


/*=========================
  client_handshake
  args: int * to_server
  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.
  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  char* name = "private";
  unlink(name);
  if(mkfifo(name, 0666) == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  printf("Client: Created private pipe\n");
  int fifo = open(ACK, O_WRONLY);
  if(fifo == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  printf("Client: Connected to WKP\n");
  *to_server = fifo;
  if(write(fifo, name, strlen(name)) == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  printf("Client: Wrote to WKP\n");
  close(fifo);
  fifo = open(name, O_RDONLY);
  if(fifo == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  int downstream = fifo;
  char message[256];
  if(read(fifo, message, 256) == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  printf("Client: Got message from server: %s\n", message);
  close(fifo);
  fifo = open(ACK, O_WRONLY);
  if(write(fifo, "Ayo", strlen("Ayo")) == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  if(unlink(name) == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  printf("Client: Deleted private pipe\n");
  return downstream;
}
