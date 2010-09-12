import fileinput, os, sys

lastwac = ''
lastepd = ''
readepd = 0
for line in fileinput.input():
	if readepd:
		lastepd = line[:-1]
		readepd = 0
	elif line[:3] == 'WAC':
		lastwac = line
		readepd = 1
	elif line[:10] == 'not solved':
		print lastepd
		
		