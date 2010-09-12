import os, sys

fout = open('/usr/src/chess/smallpotato/doc/help.txt','w')
print >> fout, """
The following is the output of running smallpotato -help on systems that
handle stderr correctly:
""" 
fout.writelines(os.popen3('/usr/src/chess/smallpotato/smallpotato -help','r')[2].readlines())
fout.close()
