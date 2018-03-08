##!/bin/env python	# ignore
# coding: utf-8
import os
import fcntl
import mmap
import json
import commands
from time import sleep
from bottle import route, run, request, HTTPResponse, template, static_file


fdRpmsg = None

@route('/static/:path#.+#', name='static')
def static(path):
	return static_file(path, root='static')

@route('/')
def root():
	return template("index")

# curl -H "Accept: application/json" -H "Content-type: application/json" -X POST -d '{"deltaX":"0.1", "deltaY":"0.1", "zoom":"1.0"}' http://192.168.1.87:8080/move
@route('/move', method='POST')
def moveEntry():
	var = request.json
	print (var)
	movePosition(var["deltaX"], var["deltaY"], var["zoom"])
	retBody = {"ret": "ok"}
	r = HTTPResponse(status=200, body=retBody)
	r.set_header('Content-Type', 'application/json')
	return r

# curl -H "Accept: application/json" -H "Content-type: application/json" -X POST -d '{"dummy":"dummy"}' http://192.168.1.87:8080/reset
@route('/reset', method='POST')
def resetEntry():
	var = request.json
	print (var)
	setPosition(0.0, 0.0, 1.0)
	retBody = {"ret": "ok"}
	r = HTTPResponse(status=200, body=retBody)
	r.set_header('Content-Type', 'application/json')
	return r

# curl -H "Accept: application/json" -H "Content-type: application/json" -X POST -d '{"dummy":"dummy"}' http://192.168.1.87:8080/color
@route('/color', method='POST')
def colorEntry():
	var = request.json
	print (var)
	randomColorMap()
	retBody = {"ret": "ok"}
	r = HTTPResponse(status=200, body=retBody)
	r.set_header('Content-Type', 'application/json')
	return r

# curl -H "Accept: application/json" -H "Content-type: application/json" -X POST -d '{"dummy":"dummy"}' http://192.168.1.87:8080/getStatus
@route('/getStatus', method='POST')
def getStatus():
	global fdRpmsg
	cmd = "get"
	# print(cmd)
	os.write(fdRpmsg, cmd)
	ret = os.read(fdRpmsg, 255)
	# ret = '{"ret":"ok", "jpegAddress":"0x3F800000", "jpegSize":"44758", "position":"X = 0.000000, Y = 0.000000, Zoom = 1.000000"}'
	# print("ret = " + ret)
	ret = json.loads(ret)
	positionStr = ret["position"]
	jpegAddress = int(ret["jpegAddress"], 16)
	jpegSize = int(ret["jpegSize"], 10)

	# save jpeg file
	fdMem = os.open("/dev/mem", os.O_RDWR | os.O_SYNC)
	mem = mmap.mmap(fdMem, jpegSize, mmap.MAP_SHARED, mmap.PROT_READ | mmap.PROT_WRITE, offset = jpegAddress)
	mem.seek(0)
	try:
		fdJpeg = open("static/00.jpg", "wb")
		fdJpeg.write(mem.read(jpegSize))
		fdJpeg.close()
	except:
		# reach here when jpeg file is being loaded
		print("conflict")
	mem.close()
	os.close(fdMem)

	retBody = {"ret": "ok", "position":positionStr}
	r = HTTPResponse(status=200, body=retBody)
	r.set_header('Content-Type', 'application/json')
	return r

def setPosition(x, y, zoom):
	global fdRpmsg
	cmd = "pos," + str(x) + "," + str(y) + "," + str(zoom)
	print(cmd)
	os.write(fdRpmsg, cmd)
	ret = os.read(fdRpmsg, 255)
	print("ret = " + ret)

def movePosition(dx, dy, zoom):
	global fdRpmsg
	cmd = "move," + str(dx) + "," + str(dy) + "," + str(zoom)
	print(cmd)
	os.write(fdRpmsg, cmd)
	ret = os.read(fdRpmsg, 255)
	print("ret = " + ret)

def randomColorMap():
	global fdRpmsg
	cmd = "color"
	print(cmd)
	os.write(fdRpmsg, cmd)
	ret = os.read(fdRpmsg, 255)
	print("ret = " + ret)

def createServerInfoFile():
	# Get my (server) IP address, and save it to js file.
	fd = open("static/js/dynamic_info.js", "w")
	# fd.write('var SERVER_URL = "http://192.168.1.87:8080/"')
	infoStr = "var SERVER_URL = 'http://" + commands.getoutput("ifconfig eth0 | awk '/inet / {print $2}' | awk -F: '{print $2}'") + ":8080/'"
	while(not ("192" in infoStr)):
		sleep(1)
		infoStr = "var SERVER_URL = 'http://" + commands.getoutput("ifconfig eth0 | awk '/inet / {print $2}' | awk -F: '{print $2}'") + ":8080/'"
	fd.write(infoStr)
	fd.close()

def main():
	while(not (os.path.exists("/dev/rpmsg0"))):
		sleep(1)

	createServerInfoFile()

	global fdRpmsg
	fdRpmsg = os.open("/dev/rpmsg0", os.O_RDWR | os.O_SYNC)

	setPosition(0.0, 0.0, 1.0)

	print('Server Start')
	# run(host='0.0.0.0', port=8080, debug=True, reloader=True)
	run(host='0.0.0.0', port=8080, debug=False, reloader=False)

	os.close(fdRpmsg)

if __name__ == '__main__':
	main()
