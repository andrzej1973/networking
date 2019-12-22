/****************** CLIENT CODE ****************/

#include <stdio.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h>


int main(){

  int network_socket; //socket descriptor

  /* create a socket witcloseh the following configuration
     domain:AFINET: IPV4 address space (AFINET6 can be used for IPV6)
     type:SOCK_STREAM: Sequenced, reliable, connection-based byte streams
     protocol: 0: select protocol automatically
  */ 
  network_socket=socket(AF_INET,SOCK_STREAM,0);

struct sockaddr_in server_address;

/* Configure settings of the server address struct */
/* Address family = Internet */
server_address.sin_family = AF_INET;
/* Set port number, using htons function to use proper byte order */
server_address.sin_port = htons(9092);
/* Set IP address to localhost */
server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

if (connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address))== -1) {
  printf("Error when connecting to socket...\n");
  return 1;
}

/* receive data from server */
char receive_buffer [255];


printf("Press <ENTER> to stop the client application\n");

int idx=0;

//while (getchar(){
while (idx<3){

  printf("Hi from while loop\n");

  if (recv(network_socket,&receive_buffer,sizeof(receive_buffer),0) == -1){
    printf("Error when receiving data from the server...\n");
    return 1;
  } else{
    printf("data received from server: %s\n",receive_buffer);
    idx++;
  }
}

close(network_socket);

  return 0;
}