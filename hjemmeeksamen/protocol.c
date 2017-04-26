#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFERSIZE 32

int client_send(int socket_fd, char* cmd) {
  char buffer[BUFFERSIZE];
  int ret;
  ret = write(socket_fd, cmd, BUFFERSIZE-1);
  buffer[BUFFERSIZE-1] = '\0';
  return ret;
}

char* client_recv(int socket_fd) {
  char* tmp_ans[BUFFERSIZE];
  read(socket_fd, tmp_ans, BUFFERSIZE);
  return tmp_ans;
}
