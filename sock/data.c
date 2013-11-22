/*****************************************************
                       data.c
    data send/receive functionality using sockets
       Authors: Mihir Joshi, Fenil Kavathia
                     csc573 NCSU
*****************************************************/

#include "data.h"

#define BACKOFF_LIMIT_CROSSED(backoff) if(backoff > 3) return 0; else backoff++;

int get_sock()
{
  int sock = socket(AF_INET, SOCK_DGRAM, 0);

  if(sock < 0)
    error("error opening socket");

  return sock;
}

int read_from(int sock, char *buffer, int buf_len, struct sockaddr_in *con_from)
{
  int bytes_read, client_len;
  struct sockaddr_in client;

  memset(buffer, '\0', buf_len);

  client_len = sizeof(client);

  bytes_read = recvfrom(sock, buffer, buf_len, 0, (struct sockaddr *) &client, &client_len);
 
#ifdef GRAN1
 printf("[log] read bytes: %d\n", bytes_read);
#endif

 *con_from = client;

 return bytes_read;
}

int write_to(int sock, char *buffer, int buf_len, char *server_addr, int port)
{
 int bytes_written;

 struct sockaddr_in sin_serversock;
 struct hostent *h_server;

 h_server = gethostbyname(server_addr);

 if(h_server == NULL)
 {
  error("No such host");
 }

 bzero((char *) &sin_serversock, sizeof(sin_serversock));

 sin_serversock.sin_family = AF_INET;
 bcopy((char *)h_server->h_addr,
 (char *) &sin_serversock.sin_addr.s_addr,
  h_server->h_length);

 sin_serversock.sin_port = htons(port);

 bytes_written = sendto(sock, buffer, buf_len, 0, (struct sockaddr * )&sin_serversock, sizeof(sin_serversock));

 if(bytes_written < 0)
  error("error writing to socket");

 return bytes_written;
}

void error(char *msg)
{
  perror(msg);

  printf("\n");

  exit(-4249);
}

void close_sock(int sock)
{
  close(sock);
}

