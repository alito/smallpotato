import sys,os
import thread



def reader(thefile):
	line = thefile.readline()
	while 1:
		print line
		line = thefile.readline()
	thread.exit()


fin = open(sys.argv[1],'r')


unin, unout = os.popen2("smallpotato")
#thread.start_new_thread(reader,(unout,))
unin.write('new\n')
unin.flush()
unin.write('force\n')

thread.start_new_thread(reader,(unout,))
games = 0

line = fin.readline()
while line != '':
	if line.strip() == '':
		line = fin.readline()
		continue
	if line[0] != '[':
		moves = line.split()
		i = 0
		while i < len(moves):
			themove = moves[i]
			dot = themove.find('.')
			if dot >= 0:
				themove = themove[dot + 1:].strip()
				if themove == '':
					i += 1
					continue
			unin.write('sp_sanmove ' + themove + '\n')
			i += 1
		
	line = fin.readline()
	
	
fin.close()

theinput = ''
while theinput.lower().strip() != 'quit':
	theinput = raw_input()
	unin.write(theinput + '\n')
	unin.flush()


unin.write('QUIT\n')
unin.flush()
unin.close()
unout.close()
