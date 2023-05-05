sudo apt-get update
sudo apt-get install -y gcc
sudo apt-get install make

make

SNIFFER_PATH=$(readlink -f output/)

echo "PATH=\"$SNIFFER_PATH:\$PATH\"" | sudo tee -a /etc/environment > /dev/null

source /etc/environment