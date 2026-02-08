//this is header file which will contain all function signature.that way my cpp code will be cleaner.
#ifndef GATEWAY.H
#define GATEWAY.H

#ifdef __cplusplus
extern "C"{
#endif

int gateway_init(int num_threads);//spawns all threads and then puts them in sleep untill a task is added in task_queue.
int gateway_submit(const char* url);//submit a url into the queue.returns request_id if succeeds, -1 if queue full.
void worker_function();//this one is the function every thread starts running.
int gateway_get_result(int request_id);//returns 1 if result available,0 if still processing,-1 if error.
void gateway_cleanup();//to close gateway
#ifdef __cplusplus
}
#endif
#endif