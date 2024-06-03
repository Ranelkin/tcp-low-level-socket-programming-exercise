#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAXPENDING 5

int main(int argc, char **argv) {
  unsigned short listen_port;     // Server port */
  int listen_sock;                // Socket descriptor for server
  int client_sock;                // Socket descriptor for client
  struct sockaddr_in listen_addr; // Local address */
  struct sockaddr_in client_addr; // Client address */

  // Check command line arguments
  if (argc != 2) {
    fprintf(stderr, "Missing parameters. Usage: %s <server-port>\n", argv[0]);
    return 1;
  }
  // Create socket for incoming connections
  if ((listen_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    perror("socket() failed");
    return 1;
  }

  listen_port =
      atoi(argv[1]); // Specified Port to listening on  for the application

  memset(&listen_addr, 0, sizeof(listen_addr));    // Zero out structure
  listen_addr.sin_family = AF_INET;                // Internet address family
  listen_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
  listen_addr.sin_port = htons(listen_port);

  // Bind to the local address
  if (bind(listen_sock, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) <
      0) {
    perror("bind() failed");
    return 1;
  }
  // Mark the socket so it will listen for incoming connections
  if (listen(listen_sock, MAXPENDING) < 0) {
    perror("listen() failed");
    return 1;
  }

  for (;;) { /* Run forever */
    socklen_t addr_len = sizeof(client_addr);

    // Wait for a client to connect */
    if ((client_sock = accept(listen_sock, (struct sockaddr *)&client_addr,
                              &addr_len)) < 0) {
      perror("accept() failed");
      return 1;
    }
    // client_sock is connected to a client
    printf("New connection from %s\n", inet_ntoa(client_addr.sin_addr));
    // Receive GET request from client before sending response
    char buffer[200];
    memset(buffer, 0, sizeof(buffer));
    int confirmation = recv(client_sock, buffer, sizeof(buffer), 0);

    if (confirmation < 0) {
      perror("Error receiving data from client");
      return 1;
    }
    // Create message to send
    time_t t = time(NULL);
    char *msg = ctime(&t);
    int msglen = strlen(msg) + 1;
    int ret;

    ret = write(client_sock, msg, msglen); // Write a message to connected
                                           // client socket anwser for request

    if (ret != msglen) {
      perror("Error during write");
      return 1;
    }
  }
  /* NOT REACHED */
  return 1;
}
