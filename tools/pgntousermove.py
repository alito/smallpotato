import sys,os

fin = open(sys.argv[1],'r')

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
			sys.stdout.write('sp_sanmove ' + themove + '\n')
			i += 1
		
	line = fin.readline()
	
	
fin.close()

