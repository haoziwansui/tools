#!/usr/bin/env python

from trash import trash
from http import http
import time
import sys
import threading


def send():
    #here are your targets
    while True:
        for host in hosts:
            for url in urls:
                ip,port = host[:-1].split(":")
                try:
                    #trash(1,2)
                    tmp = http('post',ip,int(port),url,trash('aaa','bbb'),headers)
                except Exception,e:
                    print e
        time.sleep(1)

headers = {}
hosts = open("host_web.txt").readlines()
urls = ['/index.php']
num = int(sys.argv[1])
print num
for i in range(num):
    print "start a new round of dirty"
    print 'thread %s is running...' % threading.current_thread().name
    t = threading.Thread(target=send, name='LoopThread')
    t.start()
    t.join()
    print 'thread %s ended.' % threading.current_thread().name
