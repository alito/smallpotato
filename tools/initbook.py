import sys,os


if os.path.exists('book.opn'):
	os.unlink('book.opn')


unin, unout = os.popen2("smallpotato")
#thread.start_new_thread(reader,(unout,))
unin.write('new\n')
unin.flush()
unin.write('sp_modifybook book.opn\n')


unin.write('sp_closebook\n')
unin.write('QUIT\n')
unin.flush()

