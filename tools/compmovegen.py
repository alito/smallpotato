import os, sys

fin = open(sys.argv[1],'r')
linenumber = int(sys.argv[2])

currline = []

counter = 0

while counter < linenumber:
	line = fin.readline()
	bits = line.split()
	depth = int(bits[0])
	if depth > len(currline):
		for i in range(depth - len(currline)):
			currline.insert(0,0)
	source = bits[1]
	to = bits[2]
	currline[depth-1] = (source,to)
	counter += 1

print currline


fin.close()
