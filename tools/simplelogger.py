"""
Simple logger starter
"""

import sys, logging

def configureLogging(verbosity=None, format='%(asctime)s %(levelname)-8s %(message)s'):
	level = getLevelFromVerbosity(verbosity)	
	logging.basicConfig(level=level, format=format)


def getLevelFromVerbosity(verbosity):
	"""
	Standard mapping of verbosity to log level
	"""
	loggingLevel = logging.ERROR
	if isinstance(verbosity, bool) and verbosity:
		loggingLevel = logging.DEBUG
	elif verbosity == 1:
		loggingLevel = logging.WARNING
	elif verbosity == 2:
		loggingLevel = logging.INFO
	elif verbosity >= 3:
		loggingLevel = logging.DEBUG
	
	return loggingLevel
	
