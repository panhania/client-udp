Client UDP
==========

Extremely simple UDP client made as exercise for computer networks course. Its only purpose is to download a given number of bytes through given port number. If for some unknown reason you wonderfully happen to own an UDP server that answers to queries like `GET offset bytes\n` you can try it on your own (or use the default one).

Generally, it is useless but I love my progress bar (seriously, those growing file segments...).

Running
-------
Works well with `clang` and `gcc` compilers. Just `make` to build it.

Basic execution:

    ./client-udp 40010 file.dat 1000000
    ./client-udp --timeout 250 40001 file.dat 999999
    ./client-udp -a 156.17.4.30 40008 file.dat 123456

For more info check:

    ./client-udp --help
