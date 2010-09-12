import sys,os


fin = open(sys.argv[1],'r')


unin, unout = os.popen2("smallpotato")
#thread.start_new_thread(reader,(unout,))
unin.write('new\n')
unin.flush()
unin.write('sp_modifybook book.opn\n')

games = 0

line = fin.readline()
while line != '':
	if line.strip() == '':
		line = fin.readline()
		continue
	if line[:4] == '[ECO':
		unin.write('new\n')
		currentply = 0
		games += 1
		#if games / 10 == games / 10.0:
		#	print games
		print games
	elif line[0] != '[':
		moves = line.split()
		i = 0
		while i < len(moves) and currentply < 20:
			themove = moves[i]
			dot = themove.find('.')
			if dot >= 0:
				themove = themove[dot + 1:].strip()
				if themove == '':
					i += 1
					continue
			unin.write('sp_sanmove ' + themove + '\n')
			currentply += 1
			i += 1
		
	line = fin.readline()
	
	
fin.close()

unin.write('sp_closebook\n')
unin.write('QUIT\n')
unin.flush()
