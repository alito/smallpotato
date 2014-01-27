#!/usr/bin/env python
#coding: utf8

"""
Restore the saved list of documents that were being read
"""
from __future__ import print_function, absolute_import, division

import sys, os
import logging
import subprocess

DefaultExecutable = 'smallpotato'
DefaultBook = 'book.opn'
DefaultMaxPly = 20

class EngineConnector(object):
    def __init__(self, executable, book, maxPly):
        self.games = 0
        self.maxPly = maxPly

        logging.info("Starting %s" % executable)
        self.process = subprocess.Popen([executable], stdin=subprocess.PIPE, stdout=subprocess.PIPE, close_fds=True)

        self.process.stdin.write('new\n')
        logging.info("Modifying book %s" % book)
        self.process.stdin.write('sp_modifybook %s\n' % book)


    def shutdown(self):
        logging.info("Shutting down book")
        logging.info("Processed %s games" % self.games)
        self.process.stdin.write('sp_closebook\n')
        self.process.stdin.write('QUIT\n')
        self.process.stdin.flush()
        output = self.process.stdout.read()
        return output



    def processFile(self, fin):

        for line in fin:
            if not line.strip():
                continue
            
            if line.startswith('[ECO'):
                self.process.stdin.write('new\n')
                currentply = 0
                self.games += 1

                if self.games % 100 == 0:
                    logging.info("Processed %s games" % self.games)

            elif line[0] != '[' and currentply <= self.maxPly:
                # lines like these: 
                # cxd5 Nbd7 10.Nge2 Nc5 11.Bc2 a5 12.O-O Bd7 13.a3 Nh5 14.b4 axb4 15.axb4
                moves = line.split()
                for move in moves:
                    dot = move.find('.')
                    if dot >= 0:
                        move = move[dot + 1:].strip()
                        if move == '':
                            continue
                    self.process.stdin.write('sp_sanmove %s\n' % move)
                    currentply += 1
                    if currentply > self.maxPly:
                        break


def main(args):
    from argparse import ArgumentParser
    from simplelogger import configureLogging
    
    parser = ArgumentParser(description=__doc__)
    parser.add_argument("-v", "--verbose", dest="verbosity", default=0, action="count",
                      help="Verbosity.  Invoke many times for higher verbosity")

    parser.add_argument("-b", "--book", dest="book", default=DefaultBook,
                      help="Book to create/add to (default: %(default)s)")
    parser.add_argument("-p", "--ply", dest="ply", default=DefaultMaxPly,
                      help="Number of plies from each game to process (default: %(default)s)")    
    parser.add_argument("-e", "--executable", dest="executable", default=DefaultExecutable,
                      help="Smallpotato executable to call (default: %(default)s)")

    parser.add_argument("games", nargs="+", 
        help="PGN filenames to extract data from")

    parameters = parser.parse_args(args)

    configureLogging(verbosity=parameters.verbosity)

    engine = EngineConnector(parameters.executable, parameters.book, parameters.ply)

    for gameFilename in parameters.games:
        try:
            fin = open(os.path.expanduser(gameFilename))
        except (OSError, IOError) as e:
            logging.error("Cannot open %s: %s. Skipping" % (gameFilename, e))
        else:
            engine.processFile(fin)

    engine.shutdown()

    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
