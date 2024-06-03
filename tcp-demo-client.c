#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv) {
  int ret; // return value from functions

  // Check command line arguments
  if (argc != 3) {
    fprintf(stderr,
            "Missing parameters. Usage: %s <server-name-or-ip> <server-port>\n",
            argv[0]);
    return 1;
  }
  // Address information structure
  struct addrinfo aii;
  // Set whole structure to 0s
  memset(&aii, 0, sizeof(aii));
  // A stream (TCP) connection
  aii.ai_socktype = SOCK_STREAM;
  // We do not care whether it is IPv4 or IPv6
  aii.ai_family = PF_UNSPEC;

  struct addrinfo *aio;
  // Get address information.
  // First parameter is host string, either hostname or numerical IPv4/IPv6
  // address Second parameter is port/service string, either as port number or
  // well-known identifier, e.g. http So, e.g. getaddrinfo(
  // "www.compeng.uni-frankfurt.de", "http", ... getaddrinfo( "141.2.248.1",
  // "80", ... Third parameter is input address info structure (cf. above)
  // Fourth parameter is output address info structure, a linked list of
  // potential addresses
  ret = getaddrinfo(argv[1], argv[2], &aii, &aio);
  if (ret) {
    fprintf(stderr, "Error getting address for %s:%s: %s\n", argv[1], argv[2],
            gai_strerror(ret));
    return 1;
  }

  // File descriptor for the socket
  int sock = -1;
  struct addrinfo *iter;
  // Iterate over linked list of specified output addresses,
  // use first address to which a connection can be established
  for (iter = aio; iter != NULL && sock == -1; iter = iter->ai_next) {

    sock =
        socket(iter->ai_family, iter->ai_socktype,
               iter->ai_protocol); // Iterate over over all adresses until a
                                   // connection can be established, Consisting
                                   // of protocol family, socket type and
                                   // protocol. -1 in case of failure
    if (sock < 0)
      continue; // try next address

    ret = connect(sock, iter->ai_addr,
                  iter->ai_addrlen); // Iterate over all addresses over a socket
                                     // connection
    // to the remote target adress. -1 in case of failure
    if (ret) {
      // Socket could not be connected to remote target
      close(sock); // Close socket
      sock = -1;
      continue; // try next address
    }
  }
  freeaddrinfo(
      aio); // Clean up adress information after try to establish a connection

  if (sock == -1) {
    // No connection at all could be established to remote target
    fprintf(stderr, "Unable to establish any connection to %s:%s\n", argv[1],
            argv[2]);
    return 1;
  }

  // Maximum size of incoming message
  int msglen = 100;

  // Buffer for message
  char
      buf[msglen + 1]; // One more to ensure that there is a trailing NULL char.
  
  // Create GET request, +50 is for the GET request, Host and the two \r\n
  snprintf(buf, msglen + 50, "GET / HTTP/1.1\r\nHost: %s\r\n\r\n", argv[1]);

  char response[msglen + 1];
  // Send request to server
  if (send(sock, buf, strlen(buf), 0) < 0) {
    printf("Error sending message to server\n");
    return 1;
  }
  int bytes_received = recv(sock, response, msglen + 1, 0);
  if (bytes_received < 0) {
    printf("Error receiving message from server\n");
    return 1;
  }
  printf("Server Response: %s\n", response);
  printf("Data read: '%s'\n", buf);

  // Clean up after us and close the socket
  close(sock);
  return 0;
}
