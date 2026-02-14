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

#load gateway.so 
#i don't want to hardcode the path.I will use relative path.
current_dir=os.path.dirname(os.path.abspath(__file__)) #current directory
lib_path=os.path.join(current_dir,'gateway','gateway.so') #get the relative path.gateway is subdirectory and gateway.so is the shared object.
gateway=ctypes.CDLL(lib_path) #now it will work from anywhere


#now we have to define function signature. i.e. what every function takes as arguments and what they returns.
gateway.gateway_init.argtypes=[ctypes.c_int,ctypes.c_int]
gateway.gateway_init.restype=ctypes.c_int
gateway.gateway_submit.argtypes=[ctypes.c_char_p]
gateway.gateway_submit.restype=ctypes.c_int
#gateway.gateway_get_result.argtypes=ctypes.c_int
#gateway.gateway_get_result.restype=ctypes.c_int



class Urlsubmit:
    def __init__(self):
        self.log_file="logs.txt"
    #load is called once at startup.
    def load(self,loader):#not using loader right now.may be later useful.
        print(f"Initializing gateway with {thread_count} threads...")
        result_of_gateway_init=gateway.gateway_init(thread_count,queue_length)
        if result_of_gateway_init==0:
            print("Gateway initiation successful.")
        else:
            print("Gateway initiation failed.")
        with open(self.log_file,'a') as f:
            time=datetime.now()
            f.write(f"Starting url submission at {time}...\n")
    #called before sending to server.
    def request(self,flow:http.HTTPFlow):
        url=flow.request.pretty_url
        url_id=gateway.gateway_submit(url.encode('utf-8'))
        with open(self.log_file,'a') as f:
            f.write(f"url id={url_id} has been submitted.\n")

addons=[Urlsubmit()]#This tells mitmproxy to use this addon.