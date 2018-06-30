/*
 * dummy network driver that does nothing
 *
 */


#include "net.h"


int net_open(char *bindaddr, char *playpeer) {
  bindaddr = bindaddr;
  playpeer = playpeer;
  return(-1);
}


int net_send(void *databuff, int len) {
  databuff = databuff;
  len = len;
  return(-1);
}


int net_recvpeek(void *databuff, int len) {
  databuff = databuff;
  len = len;
  return(-1);
}


int net_recv(void *databuff, int len) {
  databuff = databuff;
  len = len;
  return(-1);
}


void net_close(void) {
}
