#ifndef _NA_THREAD_H
#define _NA_THREAD_H

struct NestThread {
	void *arg1, *arg2;
};

int nthread_start(struct na_thread *thread, void *startpt(void *arg), 
				  void *data);
int nthread_join(struct na_thread *thread);

#endif

