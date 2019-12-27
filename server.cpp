/***************************************************************************
 *
 *  Project
 *                            Socket Programming Example
 *
 * Copyright (C) 2019 - 2020, Andrzej Mazur, <andrzej@mazur.info>
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://mazur.info/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/



/****************************** SERVER CODE ********************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include <arpa/inet.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <unistd.h>


/*
Debug macro definition

The do { ... } while (0) idiom ensures that the code acts like a statement (function call). 
The unconditional use of the code ensures that the compiler always checks that your debug 
code is valid — but the optimizer will remove the code when DEBUG is 0.
"fmt" is a printf format string. "..." is whatever extra arguments fmt requires 
(possibly nothing).
__VA_ARGS__ - means variable arguments
##__VA_ARGS__ - comma before the ‘##’ will be deleted. This does not happen if 
empty argument is passed, nor does it happen if the token preceding ‘##’ 
is anything other than a comma. 
see https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html for more info
*/

#define DEBUG  //comment this line to diable debug info

#ifdef DEBUG
#define debug_print(fmt, ...) \
    fprintf(stderr, "%s() in %s, line %i: " fmt "\n", \
        __func__, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define debug_print(fmt, ...) ((void)0)
#endif


#define PROGRAM_NAME "server"
#define PROGRAM_VERSION "1.0.0"

#include <pthread.h>

/*
pthread.h is required to build multi-threaded application
When compiling the program add -lpthread to the compile command. 
ie: gcc server.cpp -o server -lpthread

or in VisualStudioCode in:
$HOME/.config/Code/User/tasks.json
add:


*/

/* this variable is a reference to the second thread */
pthread_t keyPressDetectionThread;

/* this function is run by the second thread */
void *keyPressDetectionLoop(void *keyPressedVoidPtr)
{
    bool *keyPressedPtr = (bool *)keyPressedVoidPtr;
    char c;
    debug_print("Waiting for key press\n");
    c = getchar();
    *keyPressedPtr=true;
    debug_print("Key press detected\n");
  /* the function must return something - NULL is fine */
  return NULL;

}


struct helptxt {
  const char *opt;
  const char *desc;
};

static const struct helptxt helptext[] = {
  {"-h, --help",
   "Display this help message"},
  {"-v, --version",
   "Display version number and quit"},
  {"-s, --sleep-interval <microseconds>",
   "Set message send interval in microseconds, if not specified 5000000 microseconds is used"},
  { NULL, NULL }
};

void help(void)
{
  int i;
  printf("Usage:\n");
  printf("./%s [options]\n",PROGRAM_NAME);
    printf("     where option include values:\n");
  for(i = 0; helptext[i].opt; i++) {
    printf("           %-36s %s\n", helptext[i].opt, helptext[i].desc);
  }
  printf("\n");
  printf("Mail bug reports and suggestions to <andrzej@mazur.info>\n");
}

void version(void)
{
  printf("%s ver. %s\n",PROGRAM_NAME, PROGRAM_VERSION);
}

typedef struct CommandLineParameters_ {
   unsigned int sleepInterval;
   bool displayHelp;
   bool displayVersion;
   bool error;
} CommandLineParameters;

static CommandLineParameters parseParameters(int argc, char** argv) {

  CommandLineParameters flags = {
    .sleepInterval=5000000, //default sleep time set 5000000 microseconds
    .displayHelp=false,
    .displayVersion=false,
    .error=false,
  };

  if (argc==1){
   // program called with no parameters, use defeault settings
    return flags;
  }else
  {
   //parse supplied parameters, omit the first one since its a program name
   int i;
    char currentParamStr[20];

   debug_print("argc=%i\n", argc);
   for(i=1;i<argc;i++)
    {
     debug_print("parsed parameter:%s\n",argv[i]);

     if (sscanf(argv[i], "%s", &currentParamStr)!=1){
        /* Error - converting supplied parameter into string */
        debug_print("error - converting supplied parameter into string\n");
        flags.error=true;
        return flags;
      }

      if ((strcmp(currentParamStr,"-h")==0) || (strcmp(currentParamStr,"--help")==0))
      {
      //parsing help parameter
        if (argc>2)
        {
          /*Error - help parameter called not alone*/
          debug_print("error - help parameter called not alone\n");
          flags.error=true;
          return flags;
        }
        flags.displayHelp=true;
      } 
      else 
      { 
      //parsing version parameter
        if ((strcmp(currentParamStr,"-v")==0) || (strcmp(currentParamStr,"--version")==0))
        {
          if (argc>2)
          {
            /*Error - version parameter called not alone*/
            debug_print("error - version parameter called not alone\n");
            flags.error=true;
            return flags;
          }
          flags.displayVersion=true;
        } 
        else
        {
          if ((strcmp(currentParamStr,"-s")==0) || (strcmp(currentParamStr,"--sleep-interval")==0))
          {
            debug_print("parsing sleep!\n");
            i++; //move to next parameter to parse
            if (i<=(argc-1))
            {
              unsigned int sleep_val;
              if (sscanf(argv[i], "%u", &sleep_val)!=1)
              {
                /* Error - converting supplied parameter into unsigned integer */
                debug_print("Error - converting supplied parameter into unsigned integer\n");
                flags.error=true;
                return flags;
              }
              else
              {
                flags.sleepInterval=sleep_val;
              }
            }
            else
            {
              /* Error - sleep interval parameter value missing */
              debug_print("Error - sleep interval parameter value missing\n");
              flags.error=true;
              return flags;
            }
            
          }
          else
          { 
          /*error - unspecified parameter provided*/
          debug_print("error - unspecified parameter provided\n");
          flags.error=true;
          return flags;
          }
        }
      }
   }
  return flags;
  }
}

typedef struct Parameters_ {
   unsigned int sleepInterval;
} Parameters;

 Parameters SetConfiguredParametersOrQuit(CommandLineParameters flags){

   Parameters ProgramRunTimeParameters;

  /*check for errors to be done first - other flags not relevant if error = true*/
  if (flags.error==true){
    printf("%s: Supplied incorrect parameter(s)...\n\n",PROGRAM_NAME);
    help();
    exit(1);
  }

  if (flags.displayHelp==true)
  {
    help();
    exit(0);
  }

  if (flags.displayVersion==true)
  {
    version();
    exit(0);
  }

  ProgramRunTimeParameters.sleepInterval=flags.sleepInterval;
  return ProgramRunTimeParameters;

}

int main(int argc, char *argv[]){

  Parameters RunTimeParameters;
  unsigned int sleep_interval; //sleep interval in microseconds

   
  RunTimeParameters=SetConfiguredParametersOrQuit(parseParameters(argc,argv));
  sleep_interval=RunTimeParameters.sleepInterval;

  pid_t pid = getpid();
  printf("%s: Process:pid=%lu has started!\n",PROGRAM_NAME,(long)pid);

  debug_print("Sleep interval has been set to: %u\n", sleep_interval);
 

  int server_socket; //socket descriptor
  //char server_msg[255]="<-----Hallo from Server!!! You have received the server connection!!!------>";
  char server_msg[255];

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
  printf("%s: Error when binding socket to IP address and port...\n",PROGRAM_NAME);
  return 1;
}/* code */

/* Prepare for connections from clients */
/* Second argument indicates how many connections are qued before they are becoming rejected by the socket */
if (listen (server_socket, 5) == -1){/* code */
  printf("%s: Incomming connnection has been refused...\n",PROGRAM_NAME);
  return 1;
  }else
  {
    printf("%s: Successfuly prepared to listen from clients and now awaiting connection from client\n",PROGRAM_NAME);
  }
  
  int client_socket;
  struct sockaddr_in client_address;
  int client_address_len = sizeof(client_address);



/* Await for incomming connections from clients */
client_socket=accept(server_socket, (struct sockaddr *) &client_address, (unsigned int *) &client_address_len);

if (client_socket==-1){
  printf("%s: Error when connection from server has been received...\n",PROGRAM_NAME);
  return 1;
}else{
  printf("%s: Connection successfuly recieved, client socket has been opened\n",PROGRAM_NAME);
  printf("%s:   Client IP: %s\n",PROGRAM_NAME, inet_ntoa(client_address.sin_addr));
  printf("%s:   Client Port: %hu\n",PROGRAM_NAME, ntohs(client_address.sin_port));
}

printf("Press <ENTER> to stop the server application\n");


bool keyPressed=false;

/* create a second thread which executes keyPressDetectionLoop function */
if(pthread_create(&keyPressDetectionThread, NULL, keyPressDetectionLoop, &keyPressed)) {
  debug_print("Error - creating thread failed\n");
  return 1;
}
else
{
  debug_print("New Thread Successfuly created\n");
}


int idx=0;

//while (idx<3){
while (keyPressed!=true){

  sprintf(server_msg,"Msg no.: %d: <-------Hallo from the Server--------->",idx);
  /* Send a message from server to client */
  if(send(client_socket,server_msg,sizeof(server_msg),0)==-1){
    printf("%s: Sending data to the client has not been successful...\n",PROGRAM_NAME);
    return 1;
  } else
  {
    printf("%s: Message #%d, successfuly send to the client\n",PROGRAM_NAME,idx);
    printf("%s: Next message will be sent to the client in %d microseconds\n",PROGRAM_NAME,sleep_interval);
    usleep(sleep_interval);
    idx++;
  }
  if (keyPressed){
    debug_print("Key press detected, quiting %s program...\n",PROGRAM_NAME);
  }
}

close(server_socket);
close(client_socket);

return 0;

}