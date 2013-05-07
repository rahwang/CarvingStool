/*
 *
 *  CMSC 23300 / 33300 - Networks and Distributed Systems
 *
 *  main() code for chirc project
 *
 */
#include "parse_message.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

#define PORT "7776"
int numConnections = 0;

void *service_single_client(ircp ircs);
void *accept_clients(char *port);
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int quit_client(ircp ircs) {
  //check to see if the socket is already closed
  if ((send(ircs->socket," ",1,0)) == -1)
    return 0;
  pthread_mutex_lock (&lock);
  //decrement number of connections
  numConnections--;
  pthread_mutex_unlock (&lock);
  //close the socket, and free the user struct info
  close(ircs->socket);
  free(ircs->nick);
  free(ircs->user);
  free(ircs->fullname);
  free(ircs->awaymsg);
  list_destroy(&(ircs->channels));
  pthread_mutex_destroy(&(ircs->socklock));
  pthread_mutex_destroy(&(ircs->channelslock));
  return 1;
}
int main(int argc, char *argv[])
{
	int opt;
	char *port = "7776";
        passwd = NULL;
	while ((opt = getopt(argc, argv, "p:o:h")) != -1)
		switch (opt)
		{
			case 'p':
				port = strdup(optarg);
				break;
			case 'o':
				passwd = strdup(optarg);
				break;
			default:
				printf("ERROR: Unknown option -%c\n", opt);
				exit(-1);
		}

	if (!passwd)
	{
		fprintf(stderr, "ERROR: You must specify an operator password\n");
	}
	pthread_t server_thread;

        sigset_t new;
        sigemptyset (&new);
        sigaddset(&new, SIGPIPE);
        if (pthread_sigmask(SIG_BLOCK, &new, NULL) != 0)
          {
            perror("Unable to mask SIGPIPE");
            exit(-1);
          }
        
        gethostname(servhost,sizeof servhost);
        pthread_mutex_init(&lock, NULL);
	pthread_mutex_init(&userslock,NULL);
	pthread_mutex_init(&channelslock,NULL);
        //accept clients in a new thread
        if (pthread_create(&server_thread, NULL, accept_clients, port) < 0)
          {
            perror("Could not create server thread");
            exit(-1);
          }

        pthread_join(server_thread, NULL);
        
        pthread_mutex_destroy(&lock);
        pthread_mutex_destroy(&userslock);
        pthread_mutex_destroy(&channelslock);

        pthread_exit(NULL);
}

void *accept_clients(char *port) {
        int serverSocket;
        int clientSocket;
        struct addrinfo hints, *res, *p;
        char host[256];
        char service[20];
        //initiate the simclist
        list_init(&users);
        list_init(&channels);
        //allocate space for the irc struct
        struct sockaddr_storage *clientAddr;
        int yes = 1;
        socklen_t sinSize = sizeof(struct sockaddr_in);
	pthread_t worker_thread;
        void *buf;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        //fill in the structs with the server info
        if (getaddrinfo(NULL, port, &hints, &res) != 0) {
          perror("getaddrinfo() failed");
          pthread_exit(NULL);
        }
        for (p = res;p != NULL; p = p->ai_next) {
          if ((serverSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("Could not open socket");
            continue;
          }
          if ((setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) == -1) {
            	perror("Socket setsockopt() failed");
                close(serverSocket);
                continue;
          }
          //bind to the chosen socket
          if (bind(serverSocket, p->ai_addr, p->ai_addrlen) == -1)
            {
              perror("Socket bind() failed");
              close(serverSocket);
              continue;
            }
          //listen on the socket
          if ((listen(serverSocket, 5)) == -1) {
            	perror("Socket listen() failed");
                close(serverSocket);
                continue;
          }
          break;
        }
        freeaddrinfo(res);
        if (p == NULL) {
          fprintf(stderr, "Could not find a socket to bind to.\n");
          pthread_exit(NULL);
        }
        //accept clients in a while loop
        while (1) {
          clientAddr = malloc(sinSize);
          struct sockaddr_in sclientAddr;
          //wait on a client connection
          clientSocket = accept(serverSocket, (struct sockaddr *)&sclientAddr, &sinSize);
          //fill in client info
          getnameinfo(&sclientAddr, sizeof sclientAddr, host, sizeof host, service, sizeof service, 0);
          irc_info ircs;
          ircs.socket = clientSocket;
          ircs.host = host;
          pthread_create(&worker_thread, NULL, service_single_client, &ircs);
        }
        pthread_exit(NULL);
}
void *service_single_client(ircp irc_old) {
          pthread_detach(pthread_self());
          //copy over socket and host info from passed irc struct
          //create a pointer variable
	  ircp ircs = malloc(sizeof *ircs);
	  ircs->socket = irc_old->socket;
	  ircs->host = irc_old->host;
          ircs->nick = (char *) (calloc(128,sizeof(char)));
          ircs->user = (char *) (calloc(128,sizeof(char)));
          ircs->fullname = (char *) (calloc(128,sizeof(char)));
	  ircs->awaymsg = (char *) (calloc(128,sizeof(char)));
	  list_init(&(ircs->channels));
	  list_attributes_seeker(&(ircs->channels),chseeker);
	  //user is not an operator or away by default
	  ircs->op = 0;
	  ircs->away = 0;

	  pthread_mutex_init(&(ircs->socklock), NULL);
	  pthread_mutex_init(&(ircs->channelslock),NULL);
          struct sockaddr_in clientAddr;
          char msg[1024];
          int numbytes;
          int i;
          pthread_mutex_lock (&lock);
          //increment number of connections
          numConnections++;
          pthread_mutex_unlock (&lock);

          //add the user to the simclist
          pthread_mutex_lock (&userslock);
          list_append(&users,ircs);
          pthread_mutex_unlock (&userslock);

          //keep reading in input until the nick and user have been set
          while (!(strlen(ircs->nick)) || !(strlen(ircs->user))) {
            char *pbuf;
            char *curbuf;
            curbuf = (char *)(calloc(1024,sizeof(char)));
            //read in a 1024 block from the client
            numbytes = recv(ircs->socket,curbuf,1024,0);
            //separate it by \r\n
            pbuf = strtok(curbuf, "\r\n");
            while (pbuf != NULL) {
              //parse each full command
              parse_message(pbuf,ircs);
              pbuf = strtok(0,"\r\n");
            }
            free(curbuf);
          }
          //compile all the information into a welcome message
          char prefix[64];
          sprintf(msg, ":%s 001 %s :Welcome to the Internet Relay Network %s!%s@%s\r\n",servhost,ircs->nick,ircs->nick,ircs->user,ircs->host);
          send(ircs->socket, msg, strlen(msg), 0);
          sprintf(msg, ":%s 002 %s :Your host is %s, running version chirc-0.5.8rc7\r\n",servhost,ircs->nick,servhost);
          send(ircs->socket, msg, strlen(msg), 0);
          sprintf(msg, ":%s 003 %s :This server was created 2013-04-16 21:21:04\r\n",servhost,ircs->nick);
          send(ircs->socket, msg, strlen(msg), 0);
          sprintf(msg, ":%s 004 %s %s chirc-0.5.8rc7 ao motv\r\n",servhost,ircs->nick,servhost);
          send(ircs->socket, msg, strlen(msg), 0);

          //use the LUSERS and MOTD command from parse_message as part of
          //the welcome message
          parse_message("LUSERS",ircs);
          parse_message("MOTD",ircs);
	  list_init(&(ircs->channels));

          char *pbuf;
          while (1) {
            //once the client is registered keep reading in until the client
            //quits
            char tmp[512];
            char *saveptr;
            char full[1536];
            char pbuf2[512];
            char *curbuf;
            curbuf = (char *)(calloc(1024,sizeof(char)));
            //copy over the remainder command from the previous loop through
            //(if there is any)
            if (pbuf) {
              strcpy(full,pbuf);
            }
            //read in a 1024 block from the client
            numbytes = recv(ircs->socket,curbuf,1024,0);
            //if the socket is closed, run the quit_client function
            if (numbytes <= 0) {
              quit_client(ircs);
              return 0;
            }
            strcat(full,curbuf);
            //separate it by \r\n
            pbuf = strtok_r(full, "\r\n",&saveptr);
            while (1) {
              strcpy(pbuf2,pbuf);
              pbuf = strtok_r(0,"\r\n",&saveptr);
              //if we're at the end of the buffer...
              if (!(pbuf))
		{
                  //if less than the max has been read in, it is just one
                  //complete command, and should be parsed
		  if (numbytes < 1024)
		    {
		      parse_message(pbuf2, ircs);
		      pbuf2[0] = 0;		   
		    }
		  break;
		}
              //if not, simply parse the command that we are up to
              else {
                parse_message(pbuf2,ircs);
              }
            }
            //copy the leftover into the pbuffer (to be used for longer 
            //messages later, when the incomplete command needs to be stored)
	    pbuf = pbuf2;
            free(curbuf);
          }
          //quit the client once they have disconnected
          quit_client(ircs);
          return 0;
}

