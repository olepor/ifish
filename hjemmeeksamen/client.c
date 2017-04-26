#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "protocol.h"


#define BUFFERSIZE 32
#define PATH getenv("PATH")

void promt() {
  printf("cmd (? for help)> ");
}

void cd_promt() {
  printf("! .. the parent directory\n");
  printf("! / a new absolute directory\n");
  printf("!   a new directory relative to the current position\n");
  printf("! [?] this menu\n");
  printf("! [q] leave this menu\n");
}

void menu() {
  printf("! Please press a key:\n");
  printf("! [1] list content of current directory (ls)\n");
  printf("! [2] print name of current directory (pwd)\n");
  printf("! [3] change current directory (cd)\n");
  printf("! [4] get file information\n");
  printf("! [5] display file (cat)\n");
  printf("! [?] this menu\n");
  printf("! [q] quit\n");
}

int main(int argc, char* argv[]) {
  struct sockaddr_in sa;
  int socket_fd;
  char buffer[BUFFERSIZE];
  int port = atoi(argv[2]);

  if (argc < 3) {
    perror("argc");
    return EXIT_FAILURE;
  }

  if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("client: socket");
    return EXIT_FAILURE;
  }

  memset(&sa, 0, sizeof(struct sockaddr_in));
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = inet_addr(argv[1]);
  sa.sin_port = htons(port);


  int yes = 1;
  setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(int));

  if ((connect(socket_fd, (struct sockaddr *)&sa, sizeof sa)) < 0) {
    perror("client: connect");
    return EXIT_FAILURE;
  }

  printf("! You are connected to the server\n");
  menu();

  while(promt(), fgets(buffer, BUFFERSIZE, stdin)) {
    if (strcmp(buffer, "q\n") == 0) { //quit
      return EXIT_SUCCESS;
    }

    else if (strcmp(buffer, "?\n") == 0) { //menu
      menu();
    }

    else if (strcmp(buffer, "1\n") == 0) { //ls
      char* cmd = "ls";
      client_send(socket_fd, cmd);
      client_recv(socket_fd);
      continue;
    }

    else if (strcmp(buffer, "2\n") == 0) { //pwd
      char* cmd = "pwd";
      char* pwd;
      client_send(socket_fd, cmd);
      cmd = client_recv(socket_fd);
      printf("%s\n", pwd);
      continue;
    }

    else if (strcmp(buffer, "3\n") == 0) { //cd
      cd_promt();
      char cd_buffer[BUFFERSIZE];
      while (promt(), fgets(cd_buffer, BUFFERSIZE, stdin)) {
	if (strcmp(cd_buffer, "..\n") == 0) {

	}

	else if (strcmp(cd_buffer, "/\n") == 0) {

	}

	else if (strcmp(cd_buffer, " \n") == 0) {

	}

	else if (strcmp(cd_buffer, "?\n") == 0) {
	  cd_promt();

	}

	else if (strcmp(cd_buffer, "q\n") == 0) {
	  menu();
	  break;
	}

	else {
	  printf("Command does not exist.\n");
	  continue;
	}
      }
      continue;
    }

    else if (strcmp(buffer, "4\n") == 0) { //file information
      char* cmd = "fi";
      client_send(socket_fd, cmd);
      client_recv(socket_fd);
      continue;
    }

    else if (strcmp(buffer, "5\n") == 0) { //cat
      char* cmd = "cat";
      client_send(socket_fd, cmd);
      client_recv(socket_fd);
      continue;
    }

    else {
      printf("Command does not exist.\n");
      continue;
    }
    return EXIT_SUCCESS;
  }

  close(socket_fd);
  return EXIT_SUCCESS;
}
