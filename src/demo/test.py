#!/usr/bin/env python

import time
import socket, sys
import struct





#------------------------------
# main 
#------------------------------
def main():
	stServSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	try:
		stServSock.connect( ('10.0.2.15', 9995) )
	except socket.gaierror, e:
		print "Error connecting to server: %s" % e
		sys.exit(1)

	#time.sleep(3)	
	
	dataRecv = stServSock.send("hello world")


main()
