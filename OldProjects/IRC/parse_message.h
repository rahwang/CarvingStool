#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "simclist.h"
typedef struct {
  char *user;
  char *fullname;
  char *nick;
  char *host;
  int socket;
  int op;
  int away;
  char *awaymsg;
  list_t channels;
  pthread_mutex_t socklock;
  pthread_mutex_t channelslock;
} irc_info,*ircp;

typedef struct {
  char *chname;
  list_t chusers;
  list_t chvoices;
  list_t chops;
  pthread_mutex_t chuserslock;
  pthread_mutex_t chvoiceslock;
  pthread_mutex_t chopslock;
  char *chtopic;
  int moderated;
  int topic;
} channel, *chanp;

char *passwd;
char servhost[128];

int sendlock(ircp ircs, char *msg, int len, int flag);
// Setup function for finding user ircs from nickname
int seeker(const void *el, const void *key);
int chseeker(const void *el, const void *key); 
int chan_mode(irc_info *ircs, char *rcvd, char *p1);
int user_chan_mode(irc_info *ircs, char *rcvd, char *p2, chanp ch);

int parse_message(char *buf, irc_info * ircs);

int quit_client(ircp ircs);

list_t users;
list_t channels;

pthread_mutex_t userslock;
pthread_mutex_t channelslock;
pthread_mutex_t lock;
