#include "pipe_networking.h"
#include "sieve.c"

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
  printf("WKP made\n");
  int fifo = open(ACK, O_RDONLY);
  char name[10];
  if(read(fifo, name, 256) == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  printf("Got private pipe name: %s\n", name);
  int upstream = fifo;
  close(fifo);
  fifo = open(name, O_WRONLY);
  printf("%d\n", fifo);
  if(fifo == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  printf("Connected to private pipe\n");
  *to_client = fifo;
  if(write(fifo, "I gotchu", strlen("I gotchu")) == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  close(fifo);
  fifo = open(ACK, O_RDONLY);
  char message[256];
  if(read(fifo, message, 256) == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  close(fifo);
  printf("Got message from client: %s\n", message);
  printf("Handshake Complete\n");

  while(1){
    fifo = open(ACK, O_RDONLY);
    if(read(fifo, message, 256) == -1){
      printf("ERROR: %s\n", strerror(errno));
      exit(1);
    }
    close(fifo);
    char* ptr;
    int num = strtol(message, &ptr, 10);
    printf("Calculating prime #%d\n", num);
    num = sieve(num);
    char output[256];
    sprintf(output, "%d", num);
    fifo = open(name, O_WRONLY);
    if(fifo == -1){
      printf("ERROR: %s\n", strerror(errno));
      exit(1);
    }
    if(write(fifo, output, strlen(output) + 1) == -1){
      printf("ERROR: %s\n", strerror(errno));
      exit(1);
    }
    close(fifo);
  }

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
  char name[10];
  sprintf(name, "%d", getpid());
  if(mkfifo(name, 0666) == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  printf("Created private pipe\n");
  int fifo = open(ACK, O_WRONLY);
  if(fifo == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  printf("Connected to WKP\n");
  *to_server = fifo;
  if(write(fifo, name, strlen(name)) == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  printf("Wrote to WKP\n");
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
  printf("Got message from server: %s\n", message);
  close(fifo);
  fifo = open(ACK, O_WRONLY);
  if(write(fifo, "Ayo", strlen("Ayo")) == -1){
    printf("ERROR: %s\n", strerror(errno));
    exit(1);
  }
  printf("Handshake Complete\n");

  while(1){
    printf("Enter a number, n, to find the nth prime\n");
    char input[256];
    scanf("%[^\n]", input);
    getchar();
    fifo = open(ACK, O_WRONLY);
    if(write(fifo, input, strlen(input) + 1) == -1){
      printf("ERROR: %s\n", strerror(errno));
      exit(1);
    }
    close(fifo);
    fifo = open(name, O_RDONLY);
    if(fifo == -1){
      printf("ERROR: %s\n", strerror(errno));
      exit(1);
    }
    if(read(fifo, message, 256) == -1){
      printf("ERROR: %s\n", strerror(errno));
      exit(1);
    }
    close(fifo);
    printf("nth prime: %s\n", message);
  }

  return downstream;
}
