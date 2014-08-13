#encoding=utf-8

import serial, sys

s=serial.Serial(sys.argv[1], 1000000)
s.setDTR(True)
s.setRTS(True)

while 1:

	try:
		print "Waiting for FIFO"
		with open(sys.argv[2], "wb") as out:
			s.setRTS(False)	#Start capture
			while 1:
				out.write(s.read(128))

	except IOError:
		print "READ Aborted, resetting device"
		s.setRTS(True)	#Stop capture
		#Read remaining buffer
		s.read(s.inWaiting())
	
