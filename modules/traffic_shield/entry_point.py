"""
This is addon of mitmproxy.
Usage: mitmweb -s entry_point.py --listen-host 0.0.0.0 --listen-port 8080
"""
from mitmproxy import http # type: ignore cause mitmproxy is not installed in my mac.it's in vm.
from datetime import datetime
import os
import ctypes #to call cpp functions.
import config #to get values from config.py

#fetch values from config.py
thread_count=config.MAX_THREAD_COUNT
queue_length=config.MAX_QUEUE_LENGTH
max_size=config.MAX_CACHE_SIZE
ttl=config.TIME_TO_LIVE

#load all shared object file. 
#i don't want to hardcode the path.I will use relative path.
current_dir=os.path.dirname(os.path.abspath(__file__)) #current directory
lib_path_gateway=os.path.join(current_dir,'gateway','gateway.so') #get the relative path.gateway is subdirectory and gateway.so is the shared object.
gateway=ctypes.CDLL(lib_path_gateway) #now it will work from anywhere
lib_path_cache=os.path.join(current_dir,'cache','cache.so')
cache=ctypes.CDLL(lib_path_cache)
lib_path_decision=os.path.join(current_dir,'decision_engine','decision.so')
decision=ctypes.CDLL(lib_path_decision)

#now we have to define function signature. i.e. what every function takes as arguments and what they returns.
gateway.gateway_init.argtypes=[ctypes.c_int,ctypes.c_int]
gateway.gateway_init.restype=ctypes.c_int
gateway.gateway_submit.argtypes=[ctypes.c_char_p]
gateway.gateway_submit.restype=ctypes.c_int
decision.get_decision.argtypes=ctypes.c_size_t
decision.get_decision.restype=ctypes.c_bool
cache.cache_init.argtypes=[ctypes.c_int64]
cache.cache_init.restype=ctypes.c_int



class Urlsubmit:
    def __init__(self):
        self.log_file="logs.txt"
    #load is called once at startup.
    def load(self,loader):#not using loader right now.may be later useful.
        #First we initialize the gateway.
        print(f"Initializing gateway with {thread_count} threads...")
        result_of_gateway_init=gateway.gateway_init(thread_count,queue_length)
        if result_of_gateway_init==0:
            print("Gateway initiation successful.")
        else:
            print("Gateway initiation failed.")

        #We will initialize our cache here.
        result_of_cache_init=cache.cache_init(max_size,ttl)
        if result_of_cache_init==0:
            print("Cache initiation successful.")
        else:
            print("Cache initiation failed.")
        
        with open(self.log_file,'a') as f:
            time=datetime.now()
            f.write(f"Starting url submission at {time}...\n")
    #called before sending to server.
    def request(self,flow:http.HTTPFlow):
        url=flow.request.pretty_url
        url_id=gateway.gateway_submit(url.encode('utf-8'))
        #we have to somehow make the process wait untill it's result is stored in decision_engine/decision.cpp hashtable
        #or may be we can loop the get_decision function until it get's a bool.but that might cause overhead.
        #may be if the key,pair value is not found we can sleep for sometime and then query again.
        result=decision.get_decision(url_id)
        #allow/block logic.
        #ui for block
        with open(self.log_file,'a') as f:
            f.write(f"url id={url_id} has been submitted.\n")

addons=[Urlsubmit()]#This tells mitmproxy to use this addon.