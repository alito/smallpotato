import sys,os,fileinput


for line in fileinput.input():
    if line.strip() != '':
	bits = line.split(".")[1].split(" ")
	print "usermove " + bits[0]
	print "usermove " + bits[-1][:-1]

    