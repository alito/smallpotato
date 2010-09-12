import os, sys


class Tree:
	def __init__(self, parent = 0, depth = 1, ):
		self.parent = parent
		self.children = []
		self.depth = depth
	
	def addChild(self,child):
		self.children.append(child)


if len(sys.argv) < 3:
	outfile = sys.stdout
else:
	outfile = open(sys.argv[2],"w")
    
if len(sys.argv) < 2:
    infile = sys.stdin
else
    infile = open(sys.argv[1],"r")


moves = []

for line in infile.readlines():
	if line == "" or line == "\n":
		continue
	elif line[:4] == 'move':
	