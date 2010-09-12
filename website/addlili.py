import sys, os
import fileinput

uled = 0

for line in fileinput.input():
	if line[0] not in ['\n',' ','\t']:
		uled = 1
		if uled:
			print '</ul>'
		print '<p class="release">' + line[:-1] + '</p>\n<ul>'
	else:
		stripped = line.strip()
		if len(stripped) > 0 and stripped[0] != '\n':
			print '<li class="releasenote">' + line[:-1] + '</li>'
	
print '</ul>'
