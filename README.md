Small Potato - xboard/winboard compatible chess engine
======================================================

This is an old chess engine that saw more active development during 2000-2003.  The code is messy and sparsely commented.  It is also very weak compared with other engines but still very strong compared with most humans. 

Compilation follows the usual `./confgure; make`.  make install will install the binary but it is easier to just copy it where you need it.

To run using xboard as a frontend, use  
`xboard -fcp "smallpotato <options>"`  
To run it through WinBoard, you will need to add something like the following to your winboard.ini:  
`/fcp "smallpotato <options>" /fd "full\path\to\smallpotato\directory"`  
`/scp "smallpotato <options>" /sd "full\path\to\smallpotato\directory"`  
If you are using some other interface then you should read its documentation.

You will also need an opening book. You can get one from 
- https://alito.github.io/smallpotato/book64.zip for 64-bit systems
- https://alito.github.io/smallpotato/book32.zip for 32-bit systems
or you can make your own by looking at makebookfrompgn.py in the tools subdirectory. The book above was created by feeding Arturo Ochoa's collection of GM games into it.

The preferred method of configuration is to use the configuration file.  "sp.rc" is first looked
for, and if that doesn't exist, "sp.ini" is opened.  You can also specify a configuration file
through the -config commandline switch.  A default sp.rc is supplied with instructions on what each
switch means.

It is also almost fully configurable through the command line.  Run "smallpotato -help" to list them
all, or read doc/help.txt if you don't see anything when you run it (some windows versions).

Most of the program control is done through commands during the running of the program (eg to create or
modify a book).  Read commands.txt in the doc directory for more information on these.

For the opening book, Small Potato uses gdbm which you can get from http://www.gnu.org.ua/software/gdbm/  
Anything 1.x post 1.8.3 should work. Due to gdbm files not being portable between 32- and 64-bit systems, you'll need a different opening book depending on your system.

Small Potato uses clig (version 1.9.9 or greater) to take care of the command line handling, but it
isn't needed unless you are planning to change the command-line options.  It can
be gotten from: http://wsd.iitb.fhg.de/~kir/clighome/

Ideas were stolen/borrowed from lots of places.  Some of them follow:

- Crafty
- F.D. Laramée, and his how-to-write-a-chess-program tutorial from gamedev.net (http://www.gamedev.net/reference/articles/article1014.asp)
- GNU Chess
- Alpha-Beta & TT by Aske Plaat, Jonathan Schaeffer, Wim Pijls and Arie de Bruin (http://www.cs.vu.nl/~aske/Papers/tr9417.pdf)
- MTD(f) - A Minimax Algorithm faster than NegaScout, by Aske Plaat
- Research Re: search & Re-search by Aske Plaat (PhD Thesis)  (http://www.cs.vu.nl/~aske/Papers/abstr-ks.html)
- Computer Chess Web Site by Bruise Moreland http://www.seanet.com/~brucemo/chess.htm
- Pepito <http://www.winboardengines.de/pepito/>
- Amy <http://www.stethojupi.de/software.html>
- KnightCap <http://samba.org/KnightCap/> and http://syseng.anu.edu.au/~jon/papers/knightcap.ps.gz
- The Design and Implementation of the Rookie 2.0 Chess Playing Program by M.N.J. van Kervinck at http://brick.bitpit.net/~marcelk/2002/marcelk-thesis.ps.gz
- Rebel's secrets by Ed Schröder at http://members.home.nl/matador/chess840.htm


Comments, code, bugreports and patches are welcome.


  Copyright (C) 2000 Alejandro Dubrovsky

  Small Potato is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
