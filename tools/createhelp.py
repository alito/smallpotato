import os, sys


print """
The following is the output of running smallpotato -help on systems that
handle stderr correctly:
""" 
sys.stdout.writelines(os.popen3('smallpotato -help','r')[2].readlines())

