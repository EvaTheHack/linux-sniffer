sudo apt-get update
sudo apt-get install -y gcc
sudo apt-get install make

make

SNIFFER_PATH=$(readlink -f output/)

export PATH=$PATH:$SNIFFER_PATH