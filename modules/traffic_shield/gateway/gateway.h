//this is header file which will contain all function signature.that way my cpp code will be cleaner.
#ifndef GATEWAY_H
#define GATEWAY_H
#include <stddef.h> //for size_t
#ifdef __cplusplus
extern "C"{
#endif

int gateway_init(size_t number_of_threads,size_t length_of_queue); // spawns all threads and then puts them in sleep until a task is added in task_queue.
int gateway_submit(const char* url);//submit a url into the queue.returns the decision in 1/0 if succeeds, -1 if queue full.
void gateway_cleanup();//to close gateway.i will use it when gateway is opened for long but no requests are coming.it will also help to reset url_id.
#ifdef __cplusplus
}
#endif
#endif