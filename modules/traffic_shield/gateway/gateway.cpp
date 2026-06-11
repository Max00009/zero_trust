#include "gateway.h"
#include "../cache/cache.h"
#include "../decision_engine/decision.h"
#include <iostream>
#include <cstring> //bruh can't remember why i included ctring instead of string.
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <condition_variable>
#include <fstream> //just for testing.
#include <unordered_map>
#include <atomic> //for atomic type.

struct Task{
    size_t id; //every url will have it's own id so we can track
    std::string url;    //we are keeping this url as std::string instead of char* cause i think that might be helpful for url analyzer.
};

//below i initialized with static so other cpp file can't access using extern and change value.
static std::mutex queue_mutex; //to lock the queue
static std::mutex log_file_mutex; //to lock logging file.
static std::queue<Task> task_queue;   //main queue where from where our worker threads will pull.
static std::condition_variable task_cv; //conditional variable for tasks.
static std::condition_variable result_cv; //conditional variable for result.
static std::vector<std::thread> worker_threads; //vector that contains worker threads.

size_t thread_count;
size_t queue_length;
std::atomic<size_t> url_id=1; //atomic to make it thread-safe.without atomic,if two thread called it at same time they both might get 1 as starting value.
bool gateway_open;

//we will create a unordered_map here to store <id,decision>.
static std::unordered_map<size_t,bool> result_hashtable;
static std::mutex result_hashtable_mutex;

//below two functions are just dummy.main function will be defined in respective file.
int url_analyzer(std::string url){
    {
        std::lock_guard<std::mutex> lock(log_file_mutex);
        std::ofstream Logfile("delete_also.txt",std::ios::app);
        //Logfile<<task.id <<" url analizer working.."<<std::endl;
    }
    return false;
}
int threat_intelligence_module(Task task){
    {
        std::lock_guard<std::mutex> lock(log_file_mutex);
        std::ofstream Logfile("delete_also.txt",std::ios::app);
        Logfile<<task.id <<" threat intelligence module working.."<<std::endl;
    }
    return (size_t)8;
}



//this function will look into result_hashtable(which is an unordered map) and fetch decision.
void update_hashtable(size_t url_id,bool decision){
    //we have to use try_emplace() to insert key value pair.
    //cause it avoids copying so it's efficient.
    //and also if the insertion fails(for example if the key already exists) it doesn't move the arguments.so it's also safe.
    {
        std::unique_lock<std::mutex> lock(result_hashtable_mutex);  
        result_hashtable.try_emplace(url_id,decision); //insert the id and result.
    }
    result_cv.notify_all();
}

void worker_function(){
    while (true){
        Task task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex); //I used unique_lock here instead of lock_guard cause cv.wait() need the ability to lock and unlock which unique_lock provides but lock_gueard doesn't.
            /*
            cv.wait(lock,predication)->thread unlocks mutex and goes to sleep unless someone notifies one or all.
            when cv.notify_one() or cv.notify_all() occurs->thread wakes up->locks mutex->checks if predication is true.if it is true then therad proceeds.if it's false then->unlocks mutex-> thread goes to sleep again.
            */
            task_cv.wait(lock,[]{ return !task_queue.empty();}); //[] part is a lambda/anonymous function that doesn't capture anything.it just returns the bool.
            task=std::move(task_queue.front()); //instead of copying,we move.
            task_queue.pop();
        }
        //we will call cache_fetch() here.if found we will continue with next loop.if not proceed for next step.
        if(cache_fetch(task.url)!=0){
            //we are storing only 'safe' url in cache.so if a url is found in cache,it means it's safe from previous scan.so we just let it pass.
            //here we will send this url_id,true to update_hashtable function.
            update_hashtable(task.id,true);
            //and then continue with next task.
            continue;
        }


        //just for debugging.main logic will be added later.
        bool result_of_url_analysis=url_analyzer(task.url);//we are passing the url only.
        if(result_of_url_analysis!=true){
            //if there's something wrong with url then we block immedately.
            //NOTE:later I have add the feature to show the reason of failure in blocked.html
            update_hashtable(task.id,false);
            //and then continue with next task.
            continue;
        }
        size_t result_of_threat_intelligence_module=threat_intelligence_module(task);
        bool decision=decision_making(result_of_threat_intelligence_module);//decision_engine will take scores.
        update_hashtable(task.id,decision);


        //then we will call cache_insert() to update the cache.
        
    }
}

int gateway_init(size_t number_of_threads,size_t length_of_queue){
    thread_count=number_of_threads;
    queue_length=length_of_queue;
    gateway_open=true;
    for (size_t i=0;i<thread_count;i++){
        worker_threads.emplace_back(worker_function);//we are using emplace_back to create a thread directly and then push_back to worker_threads vector.
        /*we could've also done this:
        worker_threads.push_back(std::thread(worker_function));
        */
    }
    return 0;
}

int gateway_submit(const char* url){
    //create a new Task
    Task new_task;
    new_task.url=url;   //here const char* is being converted into std::string.
    new_task.id=url_id++;
    size_t request_id=new_task.id; //we are keeping a copy of id cause later we will std::move(new_task) so we can't access it's id directly.
    {
        std::lock_guard<std::mutex> lock(queue_mutex);//lock mutex to protect access to shared resources(task_queue).
        //check if queue is full
        if (task_queue.size()>queue_length){
            return -1; //means queue is full.we might later add a wait and retry here.
        }
        //otherwise add the  created task into queue.
        //we have to lock this cause our therads are reading from task_queue.
        task_queue.push(std::move(new_task)); //to avoid copying.
    
    }
    //notify sleeping therads that a new task is availbale.
    task_cv.notify_one();//we are doing this to improve cpu performance.this will make sleeping thread wake up in worker_function().

    //now we wait untill the url_id appears on result_hashtable.
    //once all analyzing done and <id,decision> added into hashtable.
    //we will extract the whole node from result_hashtable.so that after we fetch the decision that key,pair value is gone.
    //we need -c=17++ for this extract.
    {   
        std::unique_lock<std::mutex> lock(result_hashtable_mutex);
        //Sleep until request_id appears in result_hashtable.
        result_cv.wait(lock,[request_id]{
            return result_hashtable.find(request_id)!=result_hashtable.end();
        });
        //extract the value and return to entry_point.py
        auto node=result_hashtable.extract(request_id);
        if (!node.empty()){
            bool decision=node.mapped();
            return decision?1:0; //we have to return int cause that's what our signature is.
        }
        return -2;//our code will never reach this point.It's just to fix compier static analysis error.
    }
    return -2;//our code will never reach this point.It's just to fix compier static analysis error.
}






