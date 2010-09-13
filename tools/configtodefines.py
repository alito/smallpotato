#!/usr/bin/env python
"""
Convert a config.h file full of defines to command-line -D defines
"""

import os, sys, logging


def main(args):
	from required import RequiredOptionParser
	from logsetter import StandardLogSetter
	
	parser = RequiredOptionParser('%prog [-v] [-h] [options]', epilog=__doc__)
	parser.add_option("-v", "--verbose", dest="verbosity", default=0, action="count", help="Verbosity.  Invoke many times for higher verbosity")

	options, rest = parser.parse_args(args)
	if not rest:
		parser.error("Need a file full of defines to process")
	
	logger = StandardLogSetter(verbosity=options.verbosity)
	logger.startLogging()

	fin = open(os.path.expanduser(rest[0]))
	defines = []
	for line in fin:
		if line.startswith('#define'):
			bits = line.strip().split(None,2)
			if len(bits) == 1:
				logging.warn("Skipping on line: %s" % line)
			elif len(bits) == 2:
				defines.append((bits[1],None))
			else:
				defines.append((bits[1],bits[2]))

	fin.close()

	bits = []
	for variable, definition in defines:
		s = "-D%s" % variable
		if definition is not None:
			s += "=%s" % definition
		bits.append(s)
		
	print " ".join(bits)
		
	
	logger.close()	
	return 0


if __name__ == '__main__':
	try:
		import psyco
		psyco.full()
	except ImportError:
		pass
	
	sys.exit(main(sys.argv[1:]))

