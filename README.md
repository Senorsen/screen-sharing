screen-sharing
==============

A very naive web-based screen sharing (server OS must be linux), supports delta data transmit

Usage
-----
Just
    
    cd src
    make
    make install  # it'll be copied into your repo's root directory
    
    cd ..
    ./scrshare
    
    
And point your nginx(or other web server) to this directory.

Clients should use normal browsers, open the URL pointing to this directory.



Simple and naive just now
