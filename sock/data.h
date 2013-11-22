// common functionality to both client server
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>

void error(char *msg);
void close_sock(int sock);
int get_sock();
int read_from(int sock, char *buffer, int buf_len, struct sockaddr_in *con_from);
int write_to(int sock, char *buffer, int buf_len, char *server_addr, int port);
