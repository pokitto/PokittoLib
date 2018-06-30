/*
 * zBoy networking lib header
 * Copyright (C) Mateusz Viste 2010-2015
 */

#ifndef net_h_sentinel
#define net_h_sentinel

int net_open(char *bindaddr, char *playpeer);
int net_send(void *databuff, int len);
int net_recvpeek(void *databuff, int len);
int net_recv(void *databuff, int len);
void net_close(void);

#endif
