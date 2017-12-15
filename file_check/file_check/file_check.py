#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import re
import time
import sys
pathdir='/var/www/html/'
bakdir = '/tmp/bak/'  
wfilename = '/tmp/orign_file_list'
wis = 'php|php3|php4|php5|pht|phtml|phps|inc|ph3|ph4|ph5'
first_load=True     
#first_load = False
#if_check = True
if_check = False
sleep_time = 1

def get_file_info():
    cmd1 = "tar cvfz " + bakdir + "/web.tgz " + pathdir
    print os.popen(cmd1).read()
    cmd2 = "md5sum " + bakdir + "/web.tgz"
    print "filemd5 => " + os.popen(cmd2).read()
    

first_wtffff=[]
if first_load:
    if os.path.exists(bakdir):
        os.system('rm -rf bak')
    os.system('cp -r '+pathdir+' '+bakdir)
    get_file_info()

while True:
    time.sleep(sleep_time)
    try:
	######### clear the crontab#########
	os.popen("crontab -r 2>/dev/null")
	###################################
        wtffff = {}
        for fpathe,dirs,fs in os.walk(pathdir):
            for f in fs:
                ppp = os.path.join(fpathe,f)
                if os.path.isfile(ppp) and re.match(r'^\.('+wis+')$',os.path.splitext(ppp)[1]):
                    statinfo = os.stat(ppp);
                    wtffff[ppp]=statinfo.st_mtime
        if first_load:
            gofile = open(wfilename,'w')
            for ff in wtffff:
                gofile.write(ff+":"+str(wtffff[ff])+"\n")
            gofile.close()
        if not first_load:
            gofile = open(wfilename,'r')
            com_wtff=[]
            com_now_wtff = []
            for line in gofile:
                com_wtff.append(line.replace("\n",''))
                first_wtffff.append(line.replace("\n",'').split(":"))
            for ww in wtffff:
                com_now_wtff.append(":".join([ww,str(wtffff[ww])]))
            cha = list(set(com_now_wtff).difference(set(com_wtff))) #网络应用新添加的文件
            que = list(set(com_wtff).difference(set(com_now_wtff))) 
            gofile.close()
            for cc in cha:
                aaa = cc.split(":")
                vvv = time.localtime(float(aaa[1]))     #time.localtime() : (2009, 2, 17, 17, 3, 38, 1, 48, 0)
                fuckkk = str(vvv[1])+'-'+str(vvv[2])+' '+str(vvv[3])+':'+str(vvv[4])+':'+str(vvv[5])
                print aaa[0]+" |||| "+fuckkk
                if not os.path.exists(bakdir+aaa[0].replace(pathdir,'')):
                    if if_check:
                        print "the file %s not exists in bak, if remove it ? (y/n)"%aaa[0]
                        ioline = sys.stdin.readline()
                        if ioline[0].lower() == 'y':
                            os.remove(aaa[0])
                            #os.system('rm -f '+aaa[0])
                            wtffff.pop(aaa[0])
                            print aaa[0]+" has been removed"
                        else:
                            print 'skip'
                    else:
                        os.remove(aaa[0])
                        #os.system('rm -f '+aaa[0])
                        wtffff.pop(aaa[0])
                        print aaa[0]+" has been removed"
                # else:
                #   os.system('cp '+bakdir+aaa[0].replace(pathdir,'')+' '+aaa[0])
                #   wtffff[aaa[0]]=os.stat(aaa[0]).st_mtime
                #   print "the file was motified, recover file %s"%aaa[0]
            #
            for cc in que:
                aaa = cc.split(":")
                vvv = time.localtime(float(aaa[1]))
                pch = os.path.dirname(aaa[0])
                if not os.path.exists(pch):
                    os.system('mkdir -p '+pch)
                    print "the path %s was removed, and now has been recovered"%pch
                os.system('cp '+(bakdir)+aaa[0].replace(pathdir,'')+' '+aaa[0])
                wtffff[aaa[0]]=os.stat(aaa[0]).st_mtime
                print "the file was removed or motified, recover file %s"%aaa[0]
            gofile = open(wfilename,'w')
            for ff in wtffff:
                gofile.write(ff+":"+str(wtffff[ff])+"\n")
            gofile.close()
        else:
            first_load = False
    except KeyboardInterrupt:
        break
    except Exception,e:
        print e
        pass
