    phpize
    ./configure
    make && make install

    extension = wordcheck.so
    wordcheck.enable = 1
    wordcheck.deny_docs = "ecae_deny"
