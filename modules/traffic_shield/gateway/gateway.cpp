#include "gateway.h"
#include <iostream>
#include <cstring>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <condition_variable>
#include <fstream> //just for testing.

struct Task{
    int id; //every url will have it's own id so we can track
    std::string url;
};

//below i initialized with static so other cpp file can't access using extern and change value.
static std::mutex queue_mutex; //to lock the queue
static std::mutex log_file_mutex; //to lock logging file.
static std::queue<Task> task_queue;   //main queue where from where our worker threads will pull.
static std::condition_variable cv;
static std::vector<std::thread> worker_threads; //vector that contains worker threads.

int thread_count;
int queue_length;
int url_id=1;
bool gateway_open;


int url_analizer(Task task){
    {
        std::lock_guard<std::mutex> lock(log_file_mutex);
        std::ofstream Logfile("delete_also.txt",std::ios::app);
        Logfile<<task.id <<" url analizer working.."<<std::endl;
    }
    return 0;
}

int threat_intelligence_module(Task task){
    {
        std::lock_guard<std::mutex> lock(log_file_mutex);
        std::ofstream Logfile("delete_also.txt",std::ios::app);
        Logfile<<task.id <<" threat intelligence module working.."<<std::endl;
    }
    return 0;
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
            cv.wait(lock,[]{ return !task_queue.empty();}); //[] part is a lambda/anonymous function that doesn't capture anything.it just returns the bool.
            task=task_queue.front();
            task_queue.pop();
        }
        //just for debugging.main logic will be added later.
        int result_of_url_analysis=url_analizer(task);
        int result_of_threat_intelligence_module=threat_intelligence_module(task);
        
    }
}

int gateway_init(int max_thread_count,int max_queue_length){
    thread_count=max_thread_count;
    queue_length=max_queue_length;
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
    new_task.url=url;
    {
        std::lock_guard<std::mutex> lock(queue_mutex);//lock mutex to protect access to shared resources(task_queue).
        //check if queue is full
        if (task_queue.size()>queue_length){
            return -1; //means queue is full.we might later add a wait and retry here.
        }
        //even tho our threads are not sharing this resource
        //we have to lock this cause mitmproxy calls multiple thread(async).
        //it can call gate_submit() for two url at the sametime.
        new_task.id=url_id++; //give id

        //add the  created task into queue.
        //we have to lock this cause our therads are reading from task_queue.
        task_queue.push(new_task);
    
    }

    //notify sleeping therads that a new task is availbale.
    cv.notify_one();//we are doing this to improve cpu performance.this will make sleeping thread wake up in worker_function().

    return new_task.id;
}






