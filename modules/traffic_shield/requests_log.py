"""
This is addon of mitmproxy.
Usage: mitmweb -s requests_log.py --listen-host 0.0.0.0 --listen-port 8080
"""
from mitmproxy import http
from datetime import datetime
import os
class Requestslog:
    def __init__(self):
        self.log_file="logs.txt"
    #load is called once at startup.
    def load(self,loader):#not using loader right now.may be later useful.
        with open(self.log_file,'a') as f:
            f.write(f"\n=== Started at {datetime.now()} ===\n")
    #request called before forwarding to server
    def request(self,flow:http.HTTPFlow):
        timestamp=datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        method=flow.request.method
        url=flow.request.pretty_url
        host = flow.request.host
        scheme = flow.request.scheme
        path = flow.request.path
        
        log_line=(
            f"[{timestamp}]"
            f"{method} {scheme}://{host}{path}"
            f"{url}\n"
            )
        with open(self.log_file,'a') as f:
            f.write(log_line)
        print("logged")
addons=[Requestslog()]#This tells mitmproxy to use this addon.
'''mitmproxy will load requests_log.py.
    create Requestslog() instance.
    For every requests call appropriate methods accordingly'''