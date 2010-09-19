#!/usr/bin/env python
"""
Convert piece-position values into something the program can take.
Have a look at aleweights for the format.
(Very close to zero errorchecking here, so be careful)
Converted to 128 value output
"""
import os, sys, logging


def main(args):
	from optparse import OptionParser
	parser = OptionParser('%prog [-v] [-h] [options] <value file>', epilog=__doc__)
	parser.add_option("-v", "--verbose", dest="verbose", default=False, action="store_true", help="Be verbose")
	parser.add_option("-o", "--output-file", dest="outputFilename", default=None,
					  help="Output file name.  Otherwise output to <filename>.out")	
	options, rest = parser.parse_args(args)

	if not rest:
		parser.error("Need a filename to parse")
		
	if options.verbose:
		level = logging.DEBUG
	else:
		level = logging.WARNING
	logging.basicConfig(level=level)
	
	pieces = ['KING','QUEEN','ROOK','BISHOP','KNIGHT','PAWN']
	phases = ['OPEN','END','MATING']
	allmodes = []
	for p in pieces:
		for f in phases:
			allmodes.append(f + '_' + p)

	filename = os.path.expanduser(rest[0])
	fin = open(filename,'r')
	tables = {}
	for f in phases:
		tables[f] = {}

	mode = ''
	i = 0
	for line in fin.readlines():
		if line != '' and line != '\n':
			if line[:-1] in allmodes:
				mode = line[:-1]
				phase = mode.split('_')[0]
				piece = mode.split('_')[1]
				i = 0
				tables[phase][piece] = []
			else:
				values = line[:-1].split(',')
				for i in range(0,8):
					tables[phase][piece].append(values[i])
	fin.close()

	if options.outputFilename:
		outFilename = options.outputFilename
	else:
		outFilename = filename + ".out"
	
	fout = open(outFilename,'w')

	for f in phases:
		#white pieces
		for p in pieces:
			if len(tables[f][p]) != 64:
				logging.error('wrong length on ' + mode + ': ' + str(len(tables[f][p])))
				logging.error(tables[f][p])
			counter = 0
			for i in range(8):
				for j in range(8):
					fout.write(tables[f][p][counter] + ' ')
					counter += 1
				fout.write('0 0 0 0 0 0 0 0 ')
			fout.write('\n')

		#black pieces
		for p in pieces:
			if len(tables[f][p]) != 64:
				logging.error('wrong length on ' + mode + ': ' + str(len(tables[f][p])))
				logging.error(tables[f][p])
			counter = 56
			for i in range(8):
				for j in range(8):
					fout.write(tables[f][p][counter] + ' ')
					counter += 1
				fout.write('0 0 0 0 0 0 0 0 ')
				counter -= 16
			fout.write('\n')

	fout.close()

	return 0

	
if __name__ == '__main__':
	sys.exit(main(sys.argv[1:]))
	
