echo "\e[0;32mChecking for installed zeromq\e[0m";
if [ ! -d "lib/libzmq/" ]; then
    cd lib;
    git clone https://github.com/zeromq/libzmq libzmq && cd libzmq;
    mkdir build && cd build;
    cmake ..;
    if [ $SUDO -eq 0 ]; then
        make -j4 install;
    else
        sudo make -j4 install;
    fi
    cd ../../;
else
    echo "\e[0;34mzmq seems to be installed\e[0m"
fi

echo "\e[0;32mChecking for installed cppzeromq\e[0m";
if [ ! -d "lib/cppzmq/" ]; then
    git clone https://github.com/zeromq/cppzmq cppzmq && cd cppzmq;
    mkdir build && cd build;
    cmake ..;
    if [ $SUDO -eq 0 ]; then
        make -j4 install;
    else
        sudo make -j4 install;
    fi
    cd ../../;
else
    echo "\e[0;34mcppzmq seems to be installed\e[0m"
fi
