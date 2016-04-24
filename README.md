# BsidesCbrBadge2016
The BSides Canberra delegate badge code

There are 3 parts to the BSides badge code.

* The BadgeCodeGenerator which generates header files and lookup tables used
by the badge. Two things are created - the sine and cosine lookup tables which
are used by most of the demo effects. And secondly, a compressed BMP image
stored as a C array.

* The server component which gathers tweets with the hashtag #BSidesCbr and
makes it available as a text-based web page. The physical badges will connect
to this server and collect content.

* The code that gets flashed to the badge.

If the badge cannot see the SSID BSidesCTF, then it will not connect to the
badge network and therefore not have network functionality.
