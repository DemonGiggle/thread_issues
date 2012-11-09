#ifndef __H_OTHER_
#define __H_OTHER_

#define MAX_THREAD_COUNT    3
extern int get_expected_thread_count();
extern void initialize();
extern void uninitialize();
extern void notify_each_run();

#endif 
