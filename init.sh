sudo apt-get update
sudo apt-get install -y gcc

make

SNIFFER_PATH=$(readlink -f output/sniffer)

export PATH=$PATH:$SNIFFER_PATH