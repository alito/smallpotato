"""
pgn2epd.py inputfile [outputfile]
"""
import sys
import getopt

def usage():
	print __doc__

def main(argv):
	try:
		opts, args = getopt.getopt(argv, "h", 
		["help"])
	except getopt.GetoptError:
		# print help information and exit:
		usage()
		sys.exit(2)
		
	for o, a in opts:
		if o in ("-h","--help"):
			usage()
			sys.exit()
			
	if len(args) <= 0:
		usage()
		sys.exit()
	
	if args[0] == '-':
		fin = sys.stdin
	else:
		try:
			fin = open(args[0],'r')
		except IOError:
			print 'Could not open %s' % (args[0],) >> sys.stderr
		
	if len(args) <= 1 or args[1] == '-':
		fout = sys.stdout
	else:
		try:
			fout = open(args[1],'w')
		except IOError:
			print 'Could not open %s for writing' % (args[1],) >> sys.stderr
	
	lastfen = ''
	id = 0
	for line in fin.xreadlines():
		if line[:4].lower() == '[fen':
			lastfen = ' '.join(line[line.find('"')+1:line.rfind('"')].split()[:-2])
		elif line[:2] == '1.':
			bestmove = line.split()[1]
			if lastfen != '':
				epdline = lastfen + ' bm ' + bestmove + '; id "fake.%d";\n' % (id,)
				id += 1
				fout.write(epdline)
	
	fin.close()
	fout.close()
	
	
if __name__ == '__main__':
	main(sys.argv[1:])
	