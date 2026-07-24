"""
This is addon of mitmproxy.
Usage: mitmweb -s entry_point.py --listen-host 0.0.0.0 --listen-port 8080 .(optional web port)
run this^ before turning on proxy.idk why doing opposite causing repeated client disconnection.
"""
from mitmproxy import http # type: ignore cause mitmproxy is not installed in my machine.it's in vm.
from datetime import datetime
from dotenv import load_dotenv #type:ignore it will be installed in vm.
import os
import ctypes #to call cpp functions.

#first we will load the config.env file
load_dotenv()=os.path.join(os.path.dirname(os.path.abspath(__file__)),"traffic_shield_config.env")
#now fetch values from config.env.
thread_count=os.environ.get("MAX_THREAD_COUNT")
queue_length=os.environ.get("MAX_QUEUE_LENGTH")
cache_max_size=os.environ.get("MAX_CACHE_SIZE")
cache_ttl=os.environ.get("CACHE_TIME_TO_LIVE")

#load all shared object file. 
#i don't want to hardcode the path.I will use relative path.
current_dir=os.path.dirname(os.path.abspath(__file__)) #current directory
lib_path_gateway=os.path.join(current_dir,'gateway','libgateway.so') #get the relative path.gateway is subdirectory and gateway.so is the shared object.
gateway=ctypes.CDLL(lib_path_gateway) #now it will work from anywhere
lib_path_cache=os.path.join(current_dir,'cache','libcache.so')
cache=ctypes.CDLL(lib_path_cache)


#now we have to define function signature. i.e. what every function takes as arguments and what they returns.
gateway.gateway_init.argtypes=[ctypes.c_size_t,ctypes.c_size_t]
gateway.gateway_init.restype=ctypes.c_int
gateway.gateway_submit.argtypes=[ctypes.c_char_p]
gateway.gateway_submit.restype=ctypes.c_int
cache.cache_init.argtypes=[ctypes.c_size_t,ctypes.c_int64]
cache.cache_init.restype=ctypes.c_int



class Urlsubmit:
    def __init__(self):

        #self.log_file="logs.txt" DELETE

        #load the UI for blocked or error response.
        self.blocked_ui_path=os.path.join(os.path.dirname(os.path.abspath(__file__)),"ui","blocked.html")
        self.error_ui_path=os.path.join(os.path.dirname(os.path.abspath(__file__)),"ui","error.html")
        self.queue_full_ui_path=os.path.join(os.path.dirname(os.path.abspath(__file__)),"ui","queue_full.html")

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
        result_of_cache_init=cache.cache_init(cache_max_size,cache_ttl)
        if result_of_cache_init==0:
            print("Cache initiation successful.")
        else:
            print("Cache initiation failed.")
        
        # DELETE
        # with open(self.log_file,'a') as f:
        #     time=datetime.now()
        #     f.write(f"Starting url submission at {time}...\n")

    #called before sending to server.
    def request(self,flow:http.HTTPFlow):
        url=flow.request.pretty_url
        #before submitting the url which is a string object we have to convert it to byte object.
        #that's why I did url.encode('utf-8')
        decision=gateway.gateway_submit(url.encode('utf-8')) #This is the point where we jump to gateway.cpp
        
        
        #allow/block logic.
        if decision==0:
            #Load the blocked.html
            try:
                with open(self.blocked_ui_path,'r',encoding='utf-8') as f:
                    blocked=f.read()
            except FileNotFoundError:
                blocked="<h1>Blocked</h1>"

            flow.response=http.Response.make(
                403, #Forbidden
                blocked,
                {"Content-Type":"text/html"}
            )
        elif decision==-1:
            #Load the queue_full.html
            try:
                with open(self.queue_full_ui_path,'r',encoding='utf-8') as f:
                    queue_full=f.read()
            except FileNotFoundError:
                queue_full="<h1>Queue Full</h1>"

            flow.response=http.Response.make(
                429, #Too many requests
                queue_full,
                {"Content-Type":"text/html"}
            )
        elif decision==-2:
            #Load error.html
            try:
                with open(self.error_ui_path,'r',encoding='utf-8') as f:
                    error=f.read()
            except FileNotFoundError:
                error="<h1>Something went wrong</h1>"

            flow.response=http.Response.make(
                520, #Unknown error
                error,
                {"Content-Type":"text/html"}
            )
        else:
            pass

        #DELETE
        # with open(self.log_file,'a') as f:
        #     f.write(f"{url}\n{decision}\n")

addons=[Urlsubmit()]#This tells mitmproxy to use this addon.