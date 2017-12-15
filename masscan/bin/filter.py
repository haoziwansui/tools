import requests
lines = open('2.txt').readlines()
for line in lines:
    index_1 = '<address addr="'
    index_2 = '" addrtype=' 
    index_1 = line.find(index_1) +  len(index_1)
    index_2 = line.find(index_2)
    url =  line[index_1:index_2] + ":80"
    r = requests.get('http://' + url ,timeout=2)
    if  '-hxb shop' in r.text:
    	print url


