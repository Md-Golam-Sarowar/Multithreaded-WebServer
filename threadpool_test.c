/**
 * threadpool_test.c, copyright 2001 Steve Gribble
 * Just a regression test for the threadpool code.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include "threadpool.h"
#include "threadpool.c"

void dispatch_to_me(void *arg) {
  int seconds = (int) arg;
  fprintf(stdout, "  in dispatch %d\n", seconds);
  fprintf(stdout, "  done dispatch %d\n", seconds);
}


int main(int argc, char **argv) {
  threadpool tp;
  int i = 0;
 
  tp = create_threadpool(7);
  for(;i < 16;i ++) {
	  dispatch(tp, dispatch_to_me, (void *) i);		
  }
  fprintf(stdout, "**main** done first\n");
  sleep(1);
  fprintf(stdout, "\n\n");
  for(i = 0;i < 12;i ++) {
	  dispatch(tp, dispatch_to_me, (void *) i);		
  }
  fprintf(stdout, "**main done second\n");
  destroy_threadpool(tp);
  sleep(5);
  exit(-1);
}
