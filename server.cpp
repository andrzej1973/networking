/****************** SERVER CODE ****************/

#include <stdio.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h>


int main(){

  pid_t pid = getpid();
  printf("Server application with pid=%lu has started!\n",(long)pid);

  int server_socket; //socket descriptor
  char server_msg[255]="<-----Hallo from Server!!! You have received the server connection!!!------>";

  /* create a socket with the following configurationYou have received 
     domain:AFINET: IPV4 address space (AFINET6 can be used for IPV6)
     type:SOCK_STREAM: Sequenced, reliable, connection-based byte streams
     protocol: 0: select protocol automatically
  */ 
  server_socket=socket(AF_INET,SOCK_STREAM,0);

struct sockaddr_in server_address;

/* Configure settings of the server address struct */
/* Address family = Internet */
server_address.sin_family = AF_INET;
/* Set port number, using htons function to use proper byte order */
server_address.sin_port = htons(9092);
/* Set IP address to localhost */
server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

/* Bind the socket to specified IP address and port */ 
if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address))== -1) {
  printf("Error when binding socket to IP address and port...\n");
  return 1;
}/* code */

/* Prepare for connections from clients */
/* Second argument indicates how many connections are qued before they are becoming rejected by the socket */
if (listen (server_socket, 5) == -1){/* code */
  printf("Incomming connnection has been refused...\n");
  return 1;
  }else
  {
    printf("Successfuly prepared to listen from clients and now awaiting connection from client\n");
  }
  
  int client_socket;
  struct sockaddr_in client_address;
  int client_address_len = sizeof(client_address);



/* Await for incomming connections from clients */
client_socket=accept(server_socket, (struct sockaddr *) &client_address, (unsigned int *) &client_address_len);

if (client_socket==-1){
  printf("Error when connection from server has been received...\n");
  return 1;
}else{
  printf("Connection successfuly recieved, client socket has been opened\n");
  printf("  Client IP: %s\n", inet_ntoa(client_address.sin_addr));
  printf("  Client Port: %hu\n", ntohs(client_address.sin_port));
}

printf("Press <ENTER> to stop the server application\n");
unsigned int sleep_interval=5000000; //sleep interval in microseconds

int idx=0;

//while (getchar()!='\n'){
while (idx<3){
  /* Send a message from server to client */
  if(send(client_socket,server_msg,sizeof(server_msg),0)==-1){
    printf("Sending data to the client has not been successful...\n");
    return 1;
  } else
  {
    printf("Message #%d, successfuly send to the client\n",idx);
    printf("Next message will be sent to the client in %d microseconds\n",sleep_interval);
    usleep(sleep_interval);
    idx++;
  }

}

close(server_socket);
close(client_socket);

  return 0;

}