#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>

#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include "protocol.h"

#define PACKET_SIZE 32
int i;

typedef struct client_s {
  int fd;        
  char* buffer;  
  struct client_s* next;

} client_t;

client_t* head;
client_t* last;

client_t* new_client(int fd) {
  client_t* n = malloc(sizeof(client_t));
  n->fd = fd;
  n->buffer = malloc(PACKET_SIZE);
  n->next = NULL;
  return n;
}

int insert_new_client(int fd) {
  if (head == NULL) {
    head = new_client(fd);
    return 0;
  } else {
    last->next = new_client(fd);
    last = last->next;
    return 0;
  }
}

client_t* find_client(int fd) {
  client_t* tmp = head;

  while(tmp->next != NULL) {
    if (tmp->fd == fd) {
      return tmp;
    }
    tmp = tmp->next;
  }
  return NULL;
}


char* client_pwd(i) {
  char* pwd = "/INF1060/hjemmeeksamen";
  return pwd;
}

char* client_handle(int fd) {
  client_t* client = find_client(fd);
  char* tmp;
  char* pwd;
  tmp = client_recv(fd);
  printf("%s\n", tmp);
  if (!strcmp(tmp, "pwd")) {
    pwd = client_pwd(fd);
  }
  return pwd;
}

int main(int argc, char* argv[]) {
  struct sockaddr_in addr;
  fd_set masterfds, readfds, writefds, errfds;
  int listen_fd, maxfd;
  int activate = 1;

  if (argc < 2) {
    perror("argc");
    return EXIT_FAILURE;
  }

  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    return EXIT_FAILURE;
  }

  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &activate, sizeof(int)) < 0) {
    perror("setsockopt");
    return EXIT_FAILURE;
  }

  memset(&addr, 0, sizeof addr);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(atoi(argv[1]));

  if ((bind(listen_fd, (struct sockaddr *)&addr, sizeof addr)) < 0) {
    perror("bind");
    close(listen_fd);
    return EXIT_FAILURE;
  }

  if ((listen(listen_fd, SOMAXCONN)) < 0) {
    perror("server: listen");
    return EXIT_FAILURE;
  }

  FD_ZERO(&masterfds);
  FD_SET(listen_fd, &masterfds);

  printf("Now listening for connections on port %s\n", argv[1]);
  maxfd = listen_fd;

  for(;;) {
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&errfds);

    for (i = 0; i < FD_SETSIZE; i++) {
      if (FD_ISSET(i, &masterfds)) {
	FD_SET(i, &readfds);
	FD_SET(i, &writefds);
	FD_SET(i, &errfds);
      }
    }

    int ret;
    if ((ret = select(maxfd+1, &readfds, &writefds, &writefds, 0)) < 0) {
      perror("select");
      return EXIT_FAILURE;
    }


    if (FD_ISSET(listen_fd, &readfds)) {
      struct sockaddr_in client_address;
      unsigned int l;
      memset(&client_address, 0, l = sizeof(client_address));
      int newfd;
      if ((newfd = accept(listen_fd, (struct sockaddr*)&client_address, &l)) < 0) {
	perror("accept");
      } else {
	FD_SET(newfd, &masterfds);
	char address[256];
	inet_ntop(AF_INET, &client_address.sin_addr.s_addr, address, sizeof address);
	fprintf(stderr, "New connection from %s\n", address);

	maxfd = newfd > maxfd ? newfd: maxfd;

	insert_new_client(newfd);

	FD_SET(newfd, &masterfds);
	FD_CLR(listen_fd, &readfds);
      }
    }

    for (i = 0; i < FD_SETSIZE || i <= maxfd; i++) {
      if (!FD_ISSET(i, &masterfds)) {
	continue;
      }

      if (FD_ISSET(i, &readfds)) {
	ret = client_handle(i);
	client_send(i, ret);
	close(i);
	FD_CLR(i, &masterfds);
	FD_CLR(i, &readfds);
	FD_CLR(i, &errfds);
	perror("&readfds");
	fprintf(stderr, "%d has disconnected.\n", i);
	break;
      }

      if (FD_ISSET(i, &writefds)) {
	close(i);
	FD_CLR(i, &masterfds);
	FD_CLR(i, &readfds);
	FD_CLR(i, &errfds);

	perror("&writefds");
	fprintf(stderr, "%d has disconnected.\n", i);
      }
    }
  }
}
