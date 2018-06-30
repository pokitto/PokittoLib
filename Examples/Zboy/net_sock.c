/*
 * A networking wrapper around common net operations for zBoy.
 * Copyright (C) Mateusz Viste 2010-2015
 */

#include <stdio.h>
#include <string.h> /* memset() */
#include <unistd.h> /* close() */

#ifdef __zboy4linux__
  #include <arpa/inet.h>
#endif

#ifdef __zboy4windows__
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include "mingw_inet_aton.c" /* an inet_aton() substitute for windows */
  #ifndef MSG_DONTWAIT
    #define MSG_DONTWAIT 0x1000000
  #endif
#endif


#include "net.h"

struct sockaddr_in si_me, si_other;
int s;
socklen_t slen = sizeof(si_other);


int net_open(char *bindaddr, char *playpeer) {
  if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    printf("socket problem\n");
    return(-1);
  }
  memset((char *) &si_me, 0, sizeof(si_me));
  si_me.sin_family = AF_INET;
  /* if (NetPlay == 1) si_me.sin_port = htons(8765); else si_me.sin_port = htons(8764); */
  si_me.sin_port = htons(8764);
  if (bindaddr == NULL) {
    si_me.sin_addr.s_addr = htonl(INADDR_ANY); /* listen on all addresses if no bind specified */
  } else {
    if (inet_aton(bindaddr, &si_me.sin_addr) == 0) {
      printf("inet_aton() failed - bindaddr is probably not a valid IP\n");
      return(-1);
    }
  }
  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  /* if (NetPlay == 1) si_other.sin_port = htons(8764); else si_other.sin_port = htons(8765); */
  si_other.sin_port = htons(8764);
  /* if (inet_aton("127.0.0.1", &si_other.sin_addr)==0) printf("inet_aton() failed - NetPlay peer is probably not a valid IP\n"); */
  if (inet_aton(playpeer, &si_other.sin_addr) == 0) {
    printf("inet_aton() failed - NetPlay peer is probably not a valid IP\n");
    return(-1);
  }
  if (bind(s, (struct sockaddr *) &si_me, sizeof(si_me)) == -1) {
    printf("bind() problem\n");
    return(-1);
  }
  return(0);
}


int net_send(void *databuff, int len) {
  return(sendto(s, databuff, len, 0, (struct sockaddr *) &si_other, slen));
}


int net_recvpeek(void *databuff, int len) {
  return(recvfrom(s, databuff, len, MSG_PEEK | MSG_DONTWAIT, (struct sockaddr *) &si_other, &slen));
}


int net_recv(void *databuff, int len) {
  return(recvfrom(s, databuff, len, 0, (struct sockaddr *) &si_other, &slen));
}


void net_close(void) {
  close(s);
}
