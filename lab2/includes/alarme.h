#ifndef ALARME_H
#define ALARME_H

#include <unistd.h>
#include <signal.h>
#include <stdio.h>

static int flag=1;
static int connect_attempt = 0;

void atende();

#endif