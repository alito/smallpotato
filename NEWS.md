###25/09/03 - 0.6.1
* Tweaked move ordering (including addition of killer move heuristic)
* Minor tweaking of evaluation (with unknown consequences)
* More configurable settings
* Removed MTD(f) and standard alpha-beta search as search options
* Bugfixes (including K v K draw claim)

###17/09/02 - 0.6.0
* Added null-move pruning and aspiration window.
* Improved endgame evaluation (or, more precisely, it now has a separate endgame evaluation).  Same with king execution phase.
* Now claims draw on insufficient material.
* Slightly friendlier command line mode.
* Minor bugfixes (fen/epd parsing).

###18/08/02 - 0.5.1
* Now can be configured through file.
* It can now resign.
* Switched, and very slightly tweaked, default evaluation function.
* Speeded up by about 20%.

###03/08/02 - 0.5.0
* Switched default search algorithm to negascout
* Now recover a PV line for display
* Minor transposition table tweaking
* Minor fixes of bugs introduced in 0.4.0
* Now compiles out-of-the-box in clig-less systems.

###17/07/02 - 0.4.0
* Implemented move now and analyze.
* Switched to 0x88 move generation.

###11/04/02 - 0.3.3
* Added opening book.
* EPD suites and lines can be solved
* Made it portable to cygwin
* Changed default hashtable size to 32MB.
* Can now specify hashtable size in kilobytes.
* Made aleweights.out be loaded by default

###03/04/02 - 0.3.2
* No changes (reflex action release that shouldn't have 
* happened due to unkown last release)

###08/03/02 - 0.3.1
* Changed to delayed checking of check for speculated performance improvement (even though it does not seem to have materialised)
* Got rid of a couple of often used functions with array lookups for definite performance gains.
* Completed switch to move/unmove.

###26/02/02 - 0.3.0
* Changed name to Small Potato
* Changed to move/unmove instead of move/copy struct (faster)
* Implemented undo and remove for xboard/protocol 2 programs
* (Note:  Search instability noted in previous version is "meant" to be there. Caused by the nature of transposition tables, not by some bug in implementation)
* No bugfixes

###07/11/01 - 0.2.4
* Better randomised play
* Switched to 64-bit random hashes
* Lots of bugfixes, including the crash on start introduced in v0.2.3, PGN output bugfixes, and some silliness in QuiescentSearch.
* (NOTE:  Search instability still there)

###20/10/01 - 0.2.3
* Added history ordering
* Better time management.
* PGN output for game recording.
* Bugfixes

###13/10/01 - 0.2.2
* Changed GPL license
* Added quiescent search
* Bugfixes

###09/10/01 - 0.2.1
* Added internal iterative deepening
* Bugfixes

###03/10/01 - 0.2.0
* Switched to MTD(f)
* Bugfixes

###01/10/01 - 0.1.5
* Bit of documentation.
* Fixed the hashtable (maybe)

###01/10/01 - 0.1.4
* Improved packaging.
* Couple of bugfixes.

###16/04/01 - 0.1.3
* Switched to Zobrist hashing.
* Switched to negamax
* Added runtime evaluation board loading
* Lots of bugfixes (especially hashtable)

###14/03/01 - 0.1.2
* Added hashtable.
* Lots of bugfixes.

###14/02/01 - 0.1.1
* Added first move move ordering.
* Improved FICS automation.
* Better logging and recording
* Lots of bugfixes

###25/12/00 - 0.1.0
* Initial release
