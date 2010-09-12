import sys,os
#import thread
from calendar import month_abbr

def reader(thefile):
	line = thefile.readline()
	while line.find('not implemented') < 0:
		line = thefile.readline()
		print line
	thread.exit()

fin = open(sys.argv[1],'r')
fout = open(sys.argv[2],'w')
gamefinished = 1

unin, unout = os.popen2("unches")
#thread.start_new_thread(reader,(unout,))

line = fin.readline()
while line != '':
	if line.strip() == '':
		line = fin.readline()
		continue
	wherevs = line.find('vs')
	if wherevs >= 0:  #new game
		whiterating = None
		blackrating = None
		movenumber = 1
		notc = 0
		
		#find white's details
		firstparen = line[:wherevs].find('(')
		if firstparen >= 0:
			secondparen = line[:wherevs].find(')')
			if secondparen >= 0:
				whiterating = line[firstparen + 1:secondparen]
			whitename = line[:firstparen - 1]
		else:
			whitename = line[:wherevs - 1]

		#find black's details
		firstparen = line.rfind('(')
		if firstparen >= wherevs:
			secondparen = line.rfind(')')
			if secondparen >= firstparen:
				blackrating = line[firstparen + 1:secondparen]
			blackname = line[wherevs + 3:firstparen - 1]
		else:
			blackname = line[wherevs + 3:-1]

		tcline = fin.readline()
		if tcline.find('2001') >= 0:
			notc = 1
			timeline = tcline
		else:
			timeline = fin.readline()

		timebits = timeline.split()
		#print len(timebits)
		#write the header
		if gamefinished == 0:
			fout.write('\n{SEEMED TO GET DISCONNECTED; ADJOURNED} *\n\n')
		fout.write('[Event "?"]\n')
		fout.write('[Site "?"]\n')
		fout.write('[Date "' + timebits[4] + '.' + str(month_abbr.index(timebits[1])) + \
		'.' + timebits[2] + '"]\n')
		fout.write('[Time "' + timebits[3] + '"]\n')
		fout.write('[Round "-"]\n')
		fout.write('[White "' + whitename + '"]\n')
		fout.write('[Black "' + blackname + '"]\n')
		if whiterating != None:
			fout.write('[WhiteElo "' + whiterating + '"]\n')
		if blackrating != None:
			fout.write('[BlackElo "' + blackrating + '"]\n')
		if notc == 0:  #time control line
			if tcline[:3] != 'All':
				print 'Time Control line different - ' + whitename + ' (' + whiterating + \
				') vs ' + blackname + ' (' + blackrating + ')'
			else:
				tcbits = tcline.split()
				limit = tcbits[2]
				if len(tcbits) >= 6:
					increment = tcbits[5]
				else:
					increment = None
			fout.write('[TimeControl "' + limit)
			if increment != None:
				fout.write('+' + increment)
			fout.write('"]\n')
		fout.write('[Result "*"]\n\n')
		unin.write('NEW\n')
		unin.write('FORCE\n')
		unin.flush()
		currptr = 0
		gamefinished = 0

	elif line.find('{') >= 0:
		gamefinished = 1
		resultbits = line.split()
		comment = ' '.join(resultbits[1:])
		if len(resultbits[0]) + len(comment) + 2 + currptr > 76:
			fout.write('\n')
			currptr = 0
		if currptr > 0:
			fout.write(' ')
		fout.write(comment + ' ' + resultbits[0] + '\n\n')
		currptr = 0
		
	else:
		movebits = line.split()
		firstchunk = movebits[0].split('.')
		if len(firstchunk) >= 2:
			firstmove = firstchunk[1]			
			#sys.stdout.write( 'UNCHESMOVETOSAN ' + firstmove + '\n')
			unin.write('UNCHESMOVETOSAN ' + firstmove + '\n')
			unin.flush()
			"""			except IOError:
				print 'Error on ' + firstmove
				fin.close()
				fout.close()
				sys.exit(1)
			"""
			themove = unout.readline()[:-1]
			thenumber = str(movenumber)
			if len(themove) + len(thenumber) + 3 + currptr > 76:
				fout.write('\n')
				currptr = 0
			if currptr > 0:
				fout.write(' ')
			fout.write(thenumber + '. ' + themove)
			currptr += len(thenumber) + len(themove) + 3

			if len(movebits) > 1:
				secondmove = movebits[1]
				unin.write('UNCHESMOVETOSAN ' + secondmove + '\n')
				unin.flush()
				themove = unout.readline()[:-1]
				if len(themove) + currptr + 1 > 76:
					fout.write('\n')
					currptr = 0
				if currptr > 0:
					fout.write(' ')					
				fout.write(themove)
				currptr += len(themove) + 1
			movenumber += 1		
		
	line = fin.readline()
		
	
fin.close()
fout.close()
unin.write('QUIT\n')
unin.flush()
