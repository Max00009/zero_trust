#ifndef CACHE_H
#define CACHE_H
#include <stddef.h> //for size_t
#include <cstdint> //for int64_t

#ifdef __cplusplus
extern "C"{
#endif
//our functions declarations will be here.
int cache_init(size_t max_size,int64_t ttl);   //returns 0 if successful.


#ifdef __cplusplus
}
#endif

//we put only cache_init inside extern C cause that's the only function our python code(entry_point.py) will call.
//By placing other functions(which is not called from our python code) we can use c++ features(like std::string).
//extern C block is used to indicate that the functions declared inside it should use C linkage. However, std::string is a C++ type, and C does not understand C++ types or the C++ standard library.
int cache_fetch(const std::string& url);   //returns threat_score if found and 0 if not found.
int cache_insert();
#endif