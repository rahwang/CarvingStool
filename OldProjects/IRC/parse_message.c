#include "parse_message.h"

// what args to pass?
typedef int (*handler_function)(irc_info *ircp, char *rvcd, char *to_send);
// handler struct def
struct handler_entry {
    char *name;
    handler_function func;
};
int handle_NICK(irc_info *ircs, char *rcvd);
int handle_USER(irc_info *ircs, char *rcvd);
int handle_QUIT(irc_info *ircs, char *rcvd);
int handle_PRIVMSG(irc_info *ircs, char *rcvd);
int handle_NOTICE(irc_info *ircs, char *rcvd);
int handle_PING(irc_info *ircs, char *rcvd);
int handle_PONG(irc_info *ircs, char *rcvd);
int handle_LUSERS(irc_info *ircs, char *rcvd);
int handle_MOTD(irc_info *ircs, char *rcvd);
int handle_WHOIS(irc_info *ircs, char *rcvd);
int handle_JOIN(irc_info *ircs, char *rcvd);
int handle_PART(irc_info *ircs, char *rcvd);
int handle_TOPIC(irc_info *ircs, char *rcvd);
int handle_MODE(irc_info *ircs, char *rcvd);
int handle_OPER(irc_info *ircs, char *rcvd);
int handle_AWAY(irc_info *ircs, char *rcvd);
int handle_NAMES(irc_info *ircs, char *rcvd);
int handle_WHO(irc_info *ircs, char *rcvd);
int handle_LIST(irc_info *ircs, char *rcvd);

// List of handlers
struct handler_entry handlers[] = {
                                     {"NICK", handle_NICK}, 
                                     {"USER", handle_USER}, 
                                     {"QUIT", handle_QUIT}, 
                                     {"PRIVMSG", handle_PRIVMSG},
				     {"NOTICE", handle_NOTICE},
                                     {"LUSERS", handle_LUSERS},
                                     {"MOTD", handle_MOTD},
                                     {"PING", handle_PING},
                                     {"PONG", handle_PONG},
				     {"WHOIS", handle_WHOIS},
				     {"PART", handle_PART},
                                     {"JOIN", handle_JOIN},
				     {"TOPIC", handle_TOPIC},
				     {"MODE", handle_MODE},
				     {"OPER", handle_OPER},
				     {"LIST", handle_LIST},
				     {"AWAY", handle_AWAY},
				     {"NAMES", handle_NAMES},
				     {"WHO", handle_WHO}
                                  };

// Setup function for finding user ircs from nickname
int seeker(const void *el, const void *key) {
  const ircp user = (ircp)el;
  if (!(strcmp(user->nick,(char *)key)))
    return 1;
  return 0;
}

// Setup function for finding channel from chname
int chseeker(const void *el, const void *key) {
  const chanp channel = (chanp)el;
  if (!(strcmp(channel->chname,(char *)key)))
    return 1;
  return 0;
}

//thread-safe version of send function
int sendlock(ircp ircs, char *msg, int len, int flag) {
  pthread_mutex_lock(&(ircs->socklock));
  send(ircs->socket,msg,len,flag);
  pthread_mutex_unlock(&(ircs->socklock));
  return 1;
}
// Num handlers
int num_handlers = sizeof(handlers) / sizeof(struct handler_entry);

// Register a new nick. If already in use, reply with NOSUCHNICK
int handle_NICK(irc_info *ircs, char *rcvd) {
  char *p1;
  char msg[512];
  char nick[50];
  int i, j, num_ch, num_usr;
  chanp chp;
  ircp us;

  // Get nick
  p1 = strchr(rcvd, ' ')+1;
  strcpy(nick, p1);

  // Check if nick is already registered, if so, reply with error
  if ((list_seek(&users, nick))) {
    char *defnick = ((!(strlen(ircs->nick)) ? "*" : ircs->nick));
    sprintf(msg, ":%s 433 %s %s :Nickname is already in use\r\n", servhost, defnick, nick);
    sendlock(ircs, msg, strlen(msg), 0);
    return 0;
  }

  // If user is already registered, send reply with new nick
  if (strlen(ircs->nick)) {
    sprintf(msg, ":%s!%s@%s NICK :%s\r\n", ircs->nick, ircs->user, ircs->host, nick);
    sendlock(ircs, msg, strlen(msg), 0);
  }

  // Save new nick
  strncpy(ircs->nick, nick, 128);

  // Alert all user's channels of nick change
  num_ch = list_size(&(ircs->channels));
  if (num_ch <= 0) {
    return 0;
  }
  // For each channel
  for (i = 0; i < num_ch; i++) {
    chp = (chanp)list_get_at(&(ircs->channels), i);
    num_usr = list_size(&(chp->chusers));
    // For each user
    for (j = 0; j < num_usr; j++) {
      us = (ircp)(list_get_at(&(chp->chusers),j));
      if (us != ircs)
	sendlock(us,msg,strlen(msg),0);
    } 
  }
  return 0;
}

// Register a new username and fullname. 
int handle_USER(irc_info *ircs, char *rcvd) {
  char *p1; 
  char *p2;
  int len;

  // Get/save username
  p1 = strchr(rcvd, ' ');
  p2 = strchr(++p1, ' ');
  len = p2 - p1;
  strncpy(ircs->user, p1, len);

  // Get/save fullname
  p1 = strchr(rcvd, ':');
  strncpy(ircs->fullname, ++p1, 128);
  return 0;
}

// Close connection after printing client quit message
int handle_QUIT(irc_info *ircs, char *rcvd) {
  int i, j, num_ch, num_usr;
  char msg[512];
  char *p1;
  chanp chp;
  ircp us;

  // Get quit msg
  p1 = strchr(rcvd, ':');
  if (!(p1)) {
    p1 = "Client Quit";
  } else {
    p1++;
  }

  // Reply with quit msg
  sprintf(msg, "ERROR :Closing Link: %s (%s)\r\n", ircs->host, p1);
  sendlock(ircs, msg, strlen(msg), 0);

  // Alert all user's channels of exit
  num_ch = list_size(&(ircs->channels));
  if (num_ch <= 0) {			       
    quit_client(ircs);
    return 0;
  }
  sprintf(msg, ":%s!%s@%s QUIT :%s\r\n", ircs->nick, ircs->user, ircs->host, p1);
  // For each channel
  for (i = 0; i < num_ch; i++) {
    chp = (chanp)list_get_at(&(ircs->channels), i);
    num_usr = list_size(&(chp->chusers));
    // For each user
    for (j = 0; j < num_usr; j++) {
      us = (ircp)(list_get_at(&(chp->chusers),j));
      if (us != ircs)
	sendlock(us,msg,strlen(msg),0);
    } 
    list_delete(&(chp->chusers),ircs);
  }
  list_delete(&users,ircs);
  //call quit_client function from main to close the client's connection
  quit_client(ircs);
  return 0;
}

// Send privmsg to user with specified nick. If no such nick, reply with appropriate error 
int handle_PRIVMSG(irc_info *ircs, char *rcvd) {
  char *p1; 
  char *p2;
  char target[25];
  char msg[512];
  int len, i;
  ircp usr, us;
  chanp chp;

  // Get target
  p1 = strchr(rcvd, ' ');
  p2 = strchr(++p1, ' ');
  len = p2 - p1;
  strncpy(target, p1, len); 

  // Check if target is a channel
  if (target[0] == '#') {
      // If it's a channel, find target channel from given
      chp = (chanp) (list_seek(&channels, target));
      // Check if channel exists. If not, send errmsg.
      if (!(chp)) {
	sprintf(msg, ":%s 401 %s %s :No such nick/channel\r\n", servhost, ircs->nick, target);
	sendlock(ircs, msg, strlen(msg), 0);
	return 0;
      }

      sprintf(msg, ":%s 404 %s %s :Cannot send to channel\r\n", servhost, ircs->nick, target);
      // Has user joined channel? If not, send errmsg.
      if (!list_seek(&(chp->chusers), ircs->nick)) {
	sendlock(ircs, msg, strlen(msg), 0);
	return 0;
      }
      // Does user have correct privleges? If not, send errmsg.
      if (chp->moderated && !((list_seek(&(chp->chops), ircs->nick)) || (list_seek(&(chp->chvoices), ircs->nick)) || ircs->op)) {
        sendlock(ircs, msg, strlen(msg), 0);
	return 0;
      }
      // Get msg text
      p1 = strchr(rcvd, ':');

      // Send msg to each user on channel 
      int size = list_size(&(chp->chusers));
      sprintf(msg, ":%s!%s@%s PRIVMSG %s %s\r\n", ircs->nick, ircs->user, ircs->host, target, p1);
      // For each user on channel
      for (i = 0; i < size; i++) {
        us = (ircp)(list_get_at(&(chp->chusers), i));
	if (us != ircs) {
 	    sendlock(us, msg, strlen(msg), 0);
	}
      }
  // If it's not a channel, it's a single user
  } 
  else {
      // Find recipient ircp from given nick
      usr = (ircp)list_seek(&users, target);

      // If recipient registered, send privmsg
      if (usr) {
	// Get msg text
	p1 = strchr(rcvd, ':');
	sprintf(msg, ":%s!%s@%s PRIVMSG %s %s\r\n", ircs->nick, ircs->user, ircs->host, target, p1);
	sendlock(usr, msg, strlen(msg), 0);
	  
	// If recipient is away, respond to sender with away msg
	if (usr->away == 1) {
	    sprintf(msg, ":%s 301 %s %s :%s\r\n", servhost, ircs->nick, target, usr->awaymsg);
	    sendlock(ircs, msg, strlen(msg), 0);
	  }
      // If recipient doesn't exist, reply with NOSUCHNICK
      } else {
	sprintf(msg, ":%s 401 %s %s :No such nick/channel\r\n", servhost, ircs->nick, target);
	sendlock(ircs, msg, 140, 0);
      }
    }
  return 0;
}

// Send notice to appropriate user
int handle_NOTICE(irc_info *ircs, char *rcvd) {
  char *p1; 
  char *p2;
  char target[25];
  char msg[512];
  int len, i, num_usrs;
  chanp chp;
  ircp usr;

  // Get nick
  p1 = strchr(rcvd, ' ');
  p2 = strchr(++p1, ' ');
  len = p2 - p1;
  strncpy(target, p1, len);

 // Check if target is a channel
  if (target[0] == '#') {
    // If it's a channel, find target channel from given                                   
    chp = (chanp)(list_seek(&channels, target));
    
    // Check if channel exists.
    if (!chp || !list_seek(&(chp->chusers), ircs->nick)) {
      return 0;
    }
    // Check user has appropriate privleges
    if (chp->moderated && !((list_seek(&(chp->chops), ircs->nick)) || (list_seek(&(chp->chvoices), ircs->nick)) || ircs->op)) {
      return 0;
    }
    // Get msg text
    p1 = strchr(rcvd, ':');
    
    // Send msg to each user on channel
    num_usrs = list_size(&(chp->chusers));
    for (i = 0; i <num_usrs; i++) {
      usr = ((ircp)list_get_at(&(chp->chusers),i));
      sprintf(msg, ":%s!%s@%s NOTICE %s %s\r\n", ircs->nick, ircs->user, ircs->host, target, p1);
      sendlock(usr, msg, strlen(msg), 0);
    }
  // If it's not a channel, it's a single user
  } else {
    // Find recipient ircp from given nick
    usr = (ircp)list_seek(&users, target);

    // If recipient registered, send privmsg
    if (usr) {
      // Get msg text
      p1 = strchr(rcvd, ':');
      sprintf(msg, ":%s!%s@%s NOTICE %s %s\r\n", ircs->nick, ircs->user, ircs->host, target, p1);
      sendlock(usr, msg, strlen(msg), 0);
    }
  }
  return 0;
}

int handle_PING(irc_info *ircs, char *rcvd) {
  char msg[512];
  sprintf(msg, "PONG %s\r\n", servhost);
  sendlock(ircs, msg, strlen(msg), 0);
  return 0;
}

int handle_PONG(irc_info *ircs, char *rcvd) {
  return 0;
}

int handle_LUSERS(irc_info *ircs, char *rcvd) {
  char msg[512];
  int i;
  int unknowns = 0;
  int operators = 0;
  int ulen = list_size(&users);
  ircp current;

  //iterate through the users to find if any are unregistered users or operators
  for (i = 0; i < ulen;i++) {
    current = (ircp)list_get_at(&users,i);
    if (!(strlen(current->nick)) || !(strlen(current->user)))
      unknowns += 1;
    if (current->op)
      operators++;
  }
  
  //construct the replies to send using above information
  sprintf(msg, ":%s 251 %s :There are %d users and 0 services on 1 servers\r\n",
          servhost,ircs->nick,ulen-unknowns);
  sendlock(ircs,msg,strlen(msg), 0);
  sprintf(msg, ":%s 252 %s %d :operator(s) online\r\n",servhost,ircs->nick,operators);
  sendlock(ircs,msg,strlen(msg),0);
  sprintf(msg, ":%s 253 %s %d :unknown connection(s)\r\n",servhost,ircs->nick,unknowns);
  sendlock(ircs,msg,strlen(msg),0);
  sprintf(msg, ":%s 254 %s %d :channels formed\r\n",servhost,ircs->nick,list_size(&channels));
  sendlock(ircs,msg,strlen(msg),0);
  sprintf(msg, ":%s 255 %s :I have %d clients and 1 servers\r\n",servhost,ircs->nick,ulen);
  sendlock(ircs,msg,strlen(msg),0);
  return 0;
}

int handle_MOTD(irc_info *ircs, char *rcvd) {
  FILE *motd;
  
  char msg[512];
  char * line = NULL;
  //see if there is a MOTD file
  if (!(motd = fopen("motd.txt", "r"))) {
    sprintf(msg, ":%s 422 %s :MOTD File is missing\r\n",servhost,ircs->nick);
    sendlock(ircs,msg,strlen(msg),0);
    return 1;
  }
  //print the opening line of the MOTD reply
  sprintf(msg, ":%s 375 %s :- %s Message of the day - \r\n",servhost,ircs->nick,servhost);
  sendlock(ircs,msg,strlen(msg),0);
  int bytes_read;
  int len = 0;
  //for each line, add the line prefix
  sprintf(msg, ":%s 372 %s :- ",servhost,ircs->nick);
  //read in one line at a time
  while ((bytes_read = getline(&line, &len, motd)) != -1) {
    //send the line prefix, 80 characters from the line of the file, and a
    //carriage return
    sendlock(ircs,msg,strlen(msg),0);
    sendlock(ircs,line,80,0);
    sendlock(ircs,"\r\n",2,0);
  }
  //send the end of MOTD reply
  sprintf(msg, ":%s 376 %s :- End of MOTD command\r\n",servhost,ircs->nick);
  sendlock(ircs,msg,strlen(msg),0);
  return 0;
}

// Reply with info (username, fullname, server) about the specified user. NOSUCHNICK when appropriate
int handle_WHOIS(irc_info *ircs, char *rcvd) {
  char *p1;
  char msg[512];
  char nick[25];
  ircp usr;
  int i, num_ch;
  chanp chp;

  // Get nickname of sender
  p1 = strchr(rcvd, ' ')+1;
  strncpy(nick, p1, sizeof p1);
 
  // Get recipient from given nickname 
  usr = (ircp)list_seek(&users, p1);
  
  // Send recipient info and send on
  if (usr) {
    // WHOISUSER
    sprintf(msg, ":%s 311 %s %s %s %s * :%s\r\n", servhost, ircs->nick, p1, usr->user, usr->host, usr->fullname);
    sendlock(ircs, msg, strlen(msg), 0);
    // WHOISCHANNELS
      sprintf(msg, ":%s 319 %s %s :", servhost, ircs->nick, p1);
      num_ch = list_size(&(usr->channels));
      if (num_ch > 0) {
	for (i = 0; i < num_ch; i++) {
	  chp = (chanp)list_get_at(&(usr->channels), i);
	  if ((list_seek(&(chp->chops), usr->nick))) {
	    strcat(msg,"@");
	  }
	  if (list_seek(&(chp->chvoices), usr->nick)) {
	    strcat(msg,"+");
	  }
	  strcat(msg, chp->chname);
	  strcat(msg, " ");
	}
	strcat(msg, "\r\n");
	sendlock(ircs, msg, strlen(msg), 0);
      }
      // WHOISSERVER
      sprintf(msg, ":%s 312 %s %s %s %s\r\n", servhost, ircs->nick, p1, servhost, ":chirc-6.0");
      sendlock(ircs,msg,strlen(msg),0);
      // AWAY
      if (usr->away == 1) {
	sprintf(msg, ":%s 301 %s %s :%s\r\n", servhost, ircs->nick, usr->nick, usr->awaymsg);
	sendlock(ircs, msg, strlen(msg), 0);
      }
      // WHOISOPERATOR
      if (usr->op == 1) {
	sprintf(msg, ":%s 313 %s %s :is an IRC operator\r\n", servhost, ircs->nick, usr->nick);
	sendlock(ircs, msg, strlen(msg), 0);
      }
      // END
      sprintf(msg, ":%s 318 %s %s :End of WHOIS list\r\n", servhost, ircs->nick, p1);
      sendlock(ircs, msg, strlen(msg), 0);
  // If no such recipient, send error
  } else {
    sprintf(msg, ":%s 401 %s %s :No such nick/channel\r\n", servhost, ircs->nick, p1);
    sendlock(ircs, msg, strlen(msg), 0);
  }
  return 0;
}

int handle_JOIN(irc_info *ircs, char *rcvd) {
  char *p1;
  char msg[512];
  char priv[512];
  int i;
  int size;
  //get the channel part of the command
  p1 = strchr(rcvd, ' ')+1;
  chanp chp = (chanp) (list_seek(&channels,p1));
  //if the channel does not exist yet, create it
  if (!(chp)) {
    //allocate space for channel struct info
    chp = malloc(sizeof *chp);
    chp->chname = (char *) (calloc(128,sizeof(char)));
    chp->chtopic = (char *) (calloc(128,sizeof(char)));
    //add the channel name
    strcpy(chp->chname,p1);
    memset(chp->chtopic,'\0',sizeof chp->chtopic);
    //initiate the channel lists along with seeker functions
    list_init(&(chp->chusers));
    list_attributes_seeker(&(chp->chusers),seeker);
    list_init(&(chp->chvoices));
    list_attributes_seeker(&(chp->chvoices),seeker);
    list_init(&(chp->chops));
    list_attributes_seeker(&(chp->chops),seeker);
    
    //initiate the channel-specific mutex locks
    pthread_mutex_init(&(chp->chuserslock),NULL);
    pthread_mutex_init(&(chp->chvoiceslock),NULL);
    pthread_mutex_init(&(chp->chopslock),NULL);
    
    //add the user sending the command to the channel's list of users
    list_append(&(chp->chops),ircs);
    
    //add the channel to the list of channels
    pthread_mutex_lock (&channelslock);
    list_append(&channels,chp);
    pthread_mutex_unlock (&channelslock);
    
    //the channel by default is not moderated and does not have a topic
    chp->moderated = 0;
    chp->topic = 0;
  }
  //if the user is already on the channel, just return
  else {
    if (list_seek(&(chp->chusers),ircs->nick))
      return 0;
  }
  //add the user to the channel's list of users
  pthread_mutex_lock (&(chp->chuserslock));
  list_append(&(chp->chusers),ircs);
  pthread_mutex_unlock(&(chp->chuserslock));

  //add the channel to the user's list of channels
  pthread_mutex_lock(&(ircs->channelslock));
  list_append(&(ircs->channels),chp);
  pthread_mutex_unlock (&(ircs->channelslock));

  sprintf(msg, ":%s!%s@%s %s\r\n",ircs->nick,ircs->user,ircs->host,rcvd);
  sendlock(ircs,msg,strlen(msg),0);
  // If exists, send topic 
  if (strcmp(chp->chtopic,"")) {
    sprintf(msg, ":%s 332 %s %s :%s\r\n", servhost, ircs->nick, chp->chname, chp->chtopic);  
    sendlock(ircs,msg,strlen(msg),0);
  }
  //send the names replies using the NAMES handler function
  handle_NAMES(ircs, rcvd);
  sprintf(msg, ":%s!%s@%s JOIN %s\r\n",ircs->nick,ircs->user,ircs->host,p1);
  pthread_mutex_lock(&(chp->chuserslock));
  size = list_size(&(chp->chusers));
  pthread_mutex_unlock(&(chp->chuserslock));
  //send the JOIN command to every user on the channel except the one who
  //sent the command
  if (size > 1) {
      for (i = 0; i < size;i++) {
        ircp us = (ircp)list_get_at(&(chp->chusers),i);
	if (us != ircs)
            sendlock(us,msg,strlen(msg),0);
      }
    }
  return 0;
}

int handle_PART(irc_info *ircs, char *rcvd) {
  char msg[512];
  char *p1;
  char *p2;
  char name[32];
  memset(msg, '\0', sizeof(msg));

  // Get quit msg
  p2 = strchr(rcvd, ' ')+1;
  p1 = strchr(rcvd, ':');
  if (p1)
    strncpy(name,p2,p1-p2-1);
  else {
    strncpy(name,p2,32);
  }
  //if there's no quit message, it is an empty string
  if (!(p1))
    p1 = "";
  else
    p1++;
  pthread_mutex_lock(&channelslock);
  chanp ch = list_seek(&channels,name);
  pthread_mutex_unlock(&channelslock);
  //the user cannot leave what does not exist
  if (!(ch)) {
    sprintf(msg,":%s 403 %s %s :No such channel\r\n",servhost,ircs->nick,name);
    sendlock(ircs,msg,strlen(msg),0);
  }
  else {
    //or what they are not already in
    if (!(list_seek(&(ch->chusers),ircs->nick))) {
      sprintf(msg, ":%s 442 %s %s :You're not on that channel\r\n",servhost,ircs->nick,name);
      sendlock(ircs,msg,strlen(msg),0);
    }
    else {
     sprintf(msg, ":%s!%s@%s %s\r\n",ircs->nick,ircs->user,ircs->host,rcvd);

     pthread_mutex_lock(&(ch->chuserslock));
     int size = list_size(&(ch->chusers));
     pthread_mutex_unlock(&(ch->chuserslock));
     //send the part command to every user on the channel
     for (int i = 0; i < size;i++) {
        ircp us = (ircp)list_get_at(&(ch->chusers),i);
	sendlock(us,msg,strlen(msg),0);
     }
     //delete the user from the channel's list of users
     pthread_mutex_lock(&(ch->chuserslock));
     list_delete(&(ch->chusers),ircs);
     pthread_mutex_unlock(&(ch->chuserslock));
    
     //delete the channel from the user's list of channels
     pthread_mutex_lock(&(ircs->channelslock));
     list_delete(&(ircs->channels),ch);
     pthread_mutex_unlock(&(ircs->channelslock));
     //if there is nobody left on the channel, it dies a sad lonely death
     if (!(list_size(&(ch->chusers)))) {
	list_destroy(&(ch->chusers));
	list_destroy(&(ch->chvoices));
	list_destroy(&(ch->chops));
	list_delete(&channels,ch);
	pthread_mutex_destroy(&(ch->chuserslock));
	pthread_mutex_destroy(&(ch->chvoiceslock));
	pthread_mutex_destroy(&(ch->chopslock));
     }
    }
  }
}  

int handle_TOPIC(irc_info *ircs, char *rcvd)
{
  char *p1; 
  char *p2;
  char chan[25];
  char msg[128];
  int len, i;
  ircp recipient;
  
  memset(chan, '\0', sizeof(chan));
  memset(msg, '\0', sizeof(msg));
  
  // Get target
  p1 = strchr(rcvd, ' ');
  p2 = strchr(++p1, ' ');
  
  // If this is a set topic command.
  if (p2) {
      len = p2 - p1;
      strncpy(chan, p1, len);
  // If this is an inquire topic command
  } else {
    strcpy(chan, p1);
  }
  
  pthread_mutex_lock(&channelslock);
  chanp chp = (chanp) (list_seek(&channels,chan));
  pthread_mutex_unlock(&channelslock);
  // Check if channel exists. If not, send errmsg.
  if (!chp) {
    sprintf(msg, ":%s 442 %s %s :You're not on that channel\r\n", servhost, ircs->nick, chan);
    sendlock(ircs, msg, strlen(msg), 0);
    return 0;
  }

  // Has user joined channel? If not, send errmsg.
  if (!list_seek(&(chp->chusers), ircs->nick)){
    sprintf(msg, ":%s 442 %s %s :You're not on that channel\r\n", servhost, ircs->nick, chan);
    sendlock(ircs, msg, strlen(msg), 0);
    return 0;
  }

  // If this is a set topic command
  p1 = strchr(rcvd, ':');
  if (p1) {
    if (chp->topic && !((list_seek(&(chp->chops),ircs->nick)) || ircs->op)) {
      sprintf(msg,":%s 482 %s %s :You're not channel operator\r\n",servhost,ircs->nick,chp->chname);
      sendlock(ircs,msg,strlen(msg),0);
      return 0;
    }
    // Generate msg
    sprintf(msg, ":%s!%s@%s TOPIC %s :%s\r\n", ircs->nick, ircs->user, ircs->host, chan, ++p1);
    
    // Save topic
    strcpy(chp->chtopic, p1);
    
    // Send msg to each channel user
    pthread_mutex_lock(&(chp->chuserslock));
    int size = list_size(&(chp->chusers));
    pthread_mutex_unlock(&(chp->chuserslock));

    for (i = 0; i < size; i++) {
      ircp us = (ircp)(list_get_at(&(chp->chusers),i));
      sendlock(us,msg,strlen(msg),0);
    }
    // This is an inquire topic command
  } 
  else { 
    // Generate msg
    if (!strcmp(chp->chtopic, "")){
      sprintf(msg, ":%s 331 %s %s :No topic is set\r\n", servhost, ircs->nick, chan);
    } else { 
      sprintf(msg, ":%s 332 %s %s :%s\r\n", servhost, ircs->nick, chan, chp->chtopic);
    }
    
    // Send topic to inquiring user
    sendlock(ircs,msg,strlen(msg),0);
  }
  return 0;
}
int handle_MODE(irc_info *ircs, char *rcvd) {
  char msg[512];
  char *p1;
  char *p2;
  int len;
  char nick[32];
  p1 = strchr(rcvd, ' ');
  //if the argument is a channel, handle in a separate channel function
  if (p1[1] == '#') {
    chan_mode(ircs, rcvd,p1+1);
    return 1;
  }
  //get to the end of the nick argument
  p2 = strchr(++p1, ' ');
  len = p2 - p1;
  //copy the nick to the nick buffer
  strncpy(nick, p1, len);
  //the provided nick must match the user making the request
  if (strcmp(ircs->nick,nick)) {
    sprintf(msg, ":%s 502 %s :Cannot change mode for other users\r\n",servhost,ircs->nick);
    sendlock(ircs,msg,strlen(msg),0);
    return 0;
  }
  p2++;
  //see if the mode is supported
  if (p2[1] == 'o') {
    //a user can remove their oper status but not add it
    if (p2[0] == '-') {
      ircs->op = 0;
      sprintf(msg, ":%s MODE %s :%s\r\n",ircs->nick,nick,p2);
      sendlock(ircs,msg,strlen(msg),0);
    }
    return 1;
  }
  //if it is an away request, drop it silently. Otherwise, return an error
  if (p2[1] != 'a') {
    sprintf(msg, ":%s 501 %s :Unknown MODE flag\r\n",servhost,ircs->nick);
    sendlock(ircs,msg,strlen(msg),0);
    return 0;
  }
}

int chan_mode (irc_info *ircs, char *rcvd, char *p1) {
  char msg[512];
  char *p2;
  char chan[64];
  memset(chan,'\0',sizeof chan);
  int len;
  p2 = strchr(p1,' ');
  //if the channel is the only argument, return its modes
  if (!(p2)) {
    chanp ch = list_seek(&channels,p1);
    if (!(ch)) {
      sprintf(msg,":%s 403 %s %s :No such channel\r\n",servhost,ircs->nick,p1);
      sendlock(ircs,msg,strlen(msg),0);
      return 0;
    }
    //check the channel's moderated and topic mode status, and send the lsit of
    //channel modes to the target
    sprintf(msg,":%s 324 %s %s +",servhost,ircs->nick,p1);
    strcat(msg,(ch->moderated ? "m" : ""));
    strcat(msg,(ch->topic ? "t" : ""));
    strcat(msg,"\r\n");
    sendlock(ircs,msg,strlen(msg),0);
    return 1;
  }
  len = p2-p1;
  strncpy(chan,p1,len);
  chanp ch = list_seek(&channels,chan);

  if (!(ch)) {
    sprintf(msg,":%s 403 %s %s :No such channel\r\n",servhost,ircs->nick,chan);
    sendlock(ircs,msg,strlen(msg),0);
    return 0;
  }
  p2++;
  //if there is still another argument, the request is to change a user's mode
  //on a channel, which should be handled in another function
  if (strchr(p2,' ')) {
    user_chan_mode(ircs, rcvd, p2,ch);
    return 1;
  }
  
  //check to make sure it is a supported mode, and the user is a server or channel
  //operator
  if ((p2[1] == 'm' || p2[1] == 't') && !(ircs->op == 1 || (ircp)list_seek(&(ch->chops),ircs->nick))) {
    sprintf(msg, ":%s 482 %s %s :You're not channel operator\r\n",servhost,ircs->nick,ch->chname);
    sendlock(ircs,msg,strlen(msg),0);
  }
  //change the moderated or topic modes of the channel appropriately
  if (p2[1] == 'm') {
    ch->moderated = (p2[0] == '+' ? 1 : 0);
    }
  else {
    if (p2[1] == 't')
      ch->topic = (p2[0] == '+' ? 1 : 0);
    //otherwise, it is an unsupported mode
    else {
      sprintf(msg,":%s 472 %s %c :is unknown mode char to me for %s\r\n",servhost,ircs->nick,p2[1],chan);
      sendlock(ircs,msg,strlen(msg),0);
      return 0;
    }
  }
  sprintf(msg,":%s!%s@%s %s\r\n",ircs->nick,ircs->user,ircs->host,rcvd);

  pthread_mutex_lock(&(ch->chuserslock));
  int size = list_size(&(ch->chusers));
  pthread_mutex_unlock(&(ch->chuserslock));

  //relay the command to every user on the channel
  for (int i = 0; i < size; i++) {
    ircp us = list_get_at(&(ch->chusers),i);
    sendlock(us,msg,strlen(msg),0);
  }
  return 1;
}

int user_chan_mode(irc_info *ircs, char *rcvd, char *p2, chanp ch) {
  char *p3;
  char msg[512];
  char nick[64];
  int pos;
  int size;
  p3 = strchr(p2,' ')+1;
  strcpy(nick,p3);
  //the user must be a channel or server operator
  if (!(list_seek(&(ch->chops),ircs->nick)) && !(ircs->op)) {
    sprintf(msg, ":%s 482 %s %s :You're not channel operator\r\n",servhost,ircs->nick,ch->chname);
    sendlock(ircs,msg,strlen(msg),0);
    return 0;
  }
  ircp named_user = list_seek(&(ch->chusers),nick);
  //make sure the user is on the channel
  if (!(named_user)) {
    sprintf(msg, ":%s 441 %s %s %s :They aren't on that channel\r\n",servhost,ircs->nick,nick,ch->chname);
    sendlock(ircs,msg,strlen(msg),0);
    return 0;
  }
  //make sure it is a supported mode
  if (p2[1] != 'o' && p2[1] != 'v') {
    sprintf(msg, ":%s 472 %s %c :is unknown mode char to me for %s\r\n",servhost,ircs->nick,p2[1],ch->chname);
    sendlock(ircs,msg,strlen(msg),0);
    return 0;
  }
  //add or delete the given user from the list of users with voice or operator
  //privileges
  if (p2[1] == 'v') {
    if (p2[0] == '+') {
      pthread_mutex_lock (&(ch->chvoiceslock));
      list_append(&(ch->chvoices),named_user);
      pthread_mutex_unlock (&(ch->chvoiceslock));
    }
    if (p2[0] == '-') { 
      pthread_mutex_lock (&(ch->chvoiceslock));
      list_delete(&(ch->chvoices),named_user);
      pthread_mutex_unlock(&(ch->chvoiceslock));
    }
  }
  if (p2[1] == 'o') {
    if (p2[0] == '+') {
      pthread_mutex_lock (&(ch->chopslock));
      list_append(&(ch->chops),named_user);
      pthread_mutex_unlock (&(ch->chopslock));
    }
    if (p2[0] == '-') { 
      pthread_mutex_lock (&(ch->chopslock));
      list_delete(&(ch->chops),named_user);
      pthread_mutex_unlock (&(ch->chopslock));
    }
  }
  sprintf(msg, ":%s!%s@%s %s\r\n",ircs->nick,ircs->user,ircs->host,rcvd);
  pthread_mutex_lock(&(ch->chuserslock));
  size = list_size(&(ch->chusers));
  pthread_mutex_unlock(&(ch->chuserslock));

  //relay the command to every user on the channel
  for (int i = 0;i < size; i++) {
    ircp us = (ircp)list_get_at(&(ch->chusers),i);
    sendlock(us,msg,strlen(msg),0);
  }
  return 1;
}
int handle_OPER(irc_info *ircs, char *rcvd) {
  char msg[512];
  char *p1;
  char *p2;
  //get the nick argument (which is ignored)
  p1 = strchr(rcvd, ' ');
  //get the password argument
  p2 = strchr(++p1, ' ')+1;
  //make sure that the provided password is correct
  if (strcmp(p2,passwd)) {
    sprintf(msg,":%s 464 %s :Password incorrect\r\n",servhost,ircs->nick);
    sendlock(ircs,msg,strlen(msg),0);
    return 0;
  }
  //if so, give the user operator privileges, and send them the correct reply
  ircs->op = 1;
  sprintf(msg,":%s 381 %s :You are now an IRC operator\r\n",servhost,ircs->nick);
  sendlock(ircs,msg,strlen(msg),0);
  return 1;
}

int handle_LIST(irc_info *ircs, char *rcvd) {
  char *msg[512];

  // for each channel, print info
  int num_ch = list_size(&channels);
  for (int i = 0; i < num_ch; i++) {
    chanp chp = (chanp)list_get_at(&channels, i);
    int num_urs = list_size(&(chp->chusers));

    sprintf(msg,":%s 322 %s %s %d :", servhost, ircs->nick, chp->chname, num_urs);

    if (strcmp(chp->chtopic,"")) {
      strcat(msg, chp->chtopic);
    }

    strcat(msg, "\r\n");
    sendlock(ircs, msg, strlen(msg), 0);
  }
  
  sprintf(msg,":%s 323 %s :End of LIST\r\n", servhost, ircs->nick);
  sendlock(ircs, msg, strlen(msg), 0);
  return 0;
}

int handle_NAMES(irc_info *ircs, char *rcvd) {
  char *p1;
  char msg[512];
  int i, j, num_usrs, num_ch;
  
  p1 = strchr(rcvd, '#');

  // If channel specified
  if (p1) {
    chanp chp = list_seek(&channels, p1);
    // If channel does not exist, send end of names
    if (!chp) {
       sprintf(msg,":%s 366 %s * :End of NAMES list\r\n",servhost,ircs->nick);
       sendlock(ircs,msg,strlen(msg),0);
       return 0;
    }
    sprintf(msg,":%s 353 %s = %s :",servhost,ircs->nick, p1);
    // Get all users of this channel
    num_usrs = list_size(&(chp->chusers));
    for (int i = 0; i < num_usrs;i++) {
      ircp usr = (ircp)list_get_at(&(chp->chusers),i);
      if ((list_seek(&(chp->chops), usr->nick)) || usr->op) {
	strcat(msg,"@");
      }
      if (list_seek(&(chp->chvoices), usr->nick)) {
	strcat(msg,"+");
      }
      strcat(msg,usr->nick);
      strcat(msg, " ");
    }
    *(msg + strlen(msg)-1) = 0;
    strcat(msg,"\r\n");
    sendlock(ircs,msg,strlen(msg),0);
    sprintf(msg,":%s 366 %s %s :End of NAMES list\r\n",servhost,ircs->nick,p1);
    sendlock(ircs,msg,strlen(msg),0);
    // If no channel parameter, list all names, all channels
  } else {
    // Get names for each channel
    num_ch = list_size(&channels);
    for (i = 0; i < num_ch; i++) {
      chanp chp = (chanp)list_get_at(&channels, i);
      sprintf(msg,":%s 353 %s = %s :",servhost,ircs->nick, chp->chname);
      //Get all users of this channel
      num_usrs = list_size(&(chp->chusers));
      for (j = 0; j < num_usrs;j++) {
	ircp usr = (ircp)list_get_at(&(chp->chusers),j);
	if ((list_seek(&(chp->chops), usr->nick)) || usr->op) {
	  strcat(msg,"@");
	}
	if (list_seek(&(chp->chvoices), usr->nick)) {
	  strcat(msg,"+");
	}
	strcat(msg,usr->nick);
	strcat(msg, " ");
      }
      *(msg + strlen(msg)-1) = 0;
      strcat(msg,"\r\n");
      sendlock(ircs,msg,strlen(msg),0);
    }
    // Get users that are not on any channels
    num_usrs = list_size(&users);
    int ch_less = 0; // Are there users not on any channel?
    sprintf(msg,":%s 353 %s * * :",servhost,ircs->nick);
    for (i = 0; i < num_usrs; i++) {
      ircp usr = (ircp)list_get_at(&users, i);
      if (list_size(&(usr->channels)) <= 0) {
	strcat(msg, usr->nick);
	strcat(msg, " ");
	ch_less = 1;
      }
    }
    if (ch_less) {
      *(msg + strlen(msg)-1) = 0;
      strcat(msg, "\r\n");
      sendlock(ircs, msg, strlen(msg), 0);
    }
    sprintf(msg,":%s 366 %s * :End of NAMES list\r\n",servhost,ircs->nick);
    sendlock(ircs,msg,strlen(msg),0);
  }
  return 0;
}

int handle_AWAY(irc_info *ircs, char *rcvd) {
  char *p1;
  char msg[512];

  p1 = strchr(rcvd, ':');

  if (p1) {
    ircs->away = 1;
    strcpy(ircs->awaymsg, ++p1);
    sprintf(msg, ":%s 306 %s :You have been marked as being away\r\n", servhost, ircs->nick);
    sendlock(ircs, msg, strlen(msg), 0);
  }
  else {
    ircs->away = 0;
    strcpy(ircs->awaymsg, "");
    sprintf(msg, ":%s 305 %s :You are no longer marked as being away\r\n", servhost, ircs->nick);
    sendlock(ircs, msg, strlen(msg), 0);
  }
  return 0;
}

int handle_WHO(irc_info *ircs, char *rcvd) {
  char *p1;
  char msg[512];
  int i, j, num_usrs, num_ch;
  
  p1 = strchr(rcvd, '#');

  // If channel specified
  if (p1) {
    chanp chp = list_seek(&channels, p1);
    // If channel does not exist, send end of names
    if (!chp) {
       sprintf(msg,":%s 315 %s * :End of WHO list\r\n",servhost,ircs->nick);
       sendlock(ircs,msg,strlen(msg),0);
    }

    // Print info for all users of channel
    num_usrs = list_size(&(chp->chusers));
    for (i = 0; i < num_usrs;i++) {
      ircp usr = (ircp)list_get_at(&(chp->chusers),i);
      sprintf(msg, ":%s 352 %s %s %s %s %s %s ", servhost, ircs->nick, chp->chname, usr->user, usr->host, servhost, usr->nick);
      if (usr->away) {
	strcat(msg, "G");
      } else {
	strcat(msg, "H");
      }
      if (usr->op) {
	strcat(msg, "*");
      }
      if (list_seek(&(chp->chops), usr->nick)) {
	strcat(msg,"@");
      }
      if (list_seek(&(chp->chvoices), usr->nick)) {
	strcat(msg,"+");
      }
      strcat(msg, " :0 ");
      strcat(msg, usr->fullname);
      strcat(msg, "\r\n");
      sendlock(ircs,msg,strlen(msg),0);
    }
    sprintf(msg,":%s 315 %s %s :End of WHO list\r\n",servhost,ircs->nick,chp->chname);
    sendlock(ircs,msg,strlen(msg),0);
    // If no channel specified, return info for all users not sharing a channel with requester
  } else {
    num_usrs = list_size(&users);
    for (i = 0; i < num_usrs;i++) {
      ircp usr = (ircp)list_get_at(&users,i);
      // Check if any channels in common
      int shared = 0;
      int num_ck_ch = list_size(&(usr->channels));
      if (num_ck_ch <= 0) {
	shared =0;
      } else {
	for (j = 0; j < num_ck_ch; j++) {
	  chanp chp = (chanp)list_get_at(&(usr->channels),j);
	  if (list_seek(&(chp->chusers), ircs->nick)) {
	    shared = 1;
	    break;
	  }
	}
      }
      if (shared) {
	continue;
      }
      sprintf(msg, ":%s 352 %s * %s %s %s %s ", servhost, ircs->nick, usr->user, usr->host, servhost, usr->nick);
      if (usr->away) {
	strcat(msg, "G");
      } else {
	strcat(msg, "H");
      }
      if (usr->op) {
	strcat(msg, "*");
      }
      strcat(msg, " :0 ");
      strcat(msg, usr->fullname);
      strcat(msg, "\r\n");
      sendlock(ircs,msg,strlen(msg),0);
    }
    sprintf(msg,":%s 315 %s * :End of WHO list\r\n",servhost,ircs->nick);
    sendlock(ircs,msg,strlen(msg),0);
  }
  return 0;
}

int parse_message(char *buf, irc_info * ircs)
{
  char cmd[512]; // Store command
  char *p1;
  char servhost[256];
  gethostname(servhost,sizeof servhost);

  char to_send[512]; // Pointers for use with strchr() 
  int len, i;

  list_attributes_seeker(&users, seeker);
  list_attributes_seeker(&channels,chseeker);
  // Initialize cmd
  memset(cmd, '\0', sizeof(cmd));
  memset(to_send, '\0', sizeof(cmd));
  
  // Get command (Does not account for prefixes)
  p1 = strchr(buf, ' ');
  if (p1)
    len = p1 - buf;
  else
   len = strlen(buf);
  strncpy(cmd, buf, len);

  // Determine appropriate handler
  for(i=0; i<num_handlers; i++)
    if (!strcmp(handlers[i].name, cmd))
    {
        handlers[i].func(ircs, buf, to_send);
        break;
    }

  // If no handler matched, reply with unknown cmd error
  if(i == num_handlers)
    {
      sprintf(to_send, ":%s 421 %s %s :Unknown command\r\n", servhost,ircs->nick,cmd);
      sendlock(ircs,to_send,strlen(to_send),0);
      return 1;
    }
  return 0;
}
