#!/bin/bash -ev

mkdir -p ~/.vcore
echo "rpcuser=username" >>~/.vcore/vcore.conf
echo "rpcpassword=`head -c 32 /dev/urandom | base64`" >>~/.vcore/vcore.conf
