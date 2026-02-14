#ifndef CACHE_H
#define CACHE_H
#include <cstdint>//for int64_t

#ifdef __cplusplus
extern "C"{
#endif
//our functions declarations will be here.
int cache_init(int max_size,int64_t ttl);   //returns 0 if successful.
int cache_fetch(const char* url);   //returns threat_score if found and 0 if not found.
int cache_insert();






#ifdef __cplusplus
}
#endif


#endif