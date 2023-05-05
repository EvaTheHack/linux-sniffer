# Linux sniffer
### Service that collects statistic about network traffic and store to txt

## How to setup

```
git clone https://github.com/EvaTheHack/linux-sniffer.git
apt-get install make
chmod +x init.sh
chmod +x init_daemon.sh
./init.sh
sudo ./init_daemon.sh
```

## Usage 
### Before start daemon must be started
### All commands for work with daemon:
```
service daemon_sniffer start  - start a daemon
service daemon_sniffer stop   - stop a daemon
service daemon_sniffer status - show status of daemon
service daemon_sniffer reload - stop and start daemon
```

### Another commands for cli:

```
start - start sniffing 
stop  - stop sniffing 
show [ip] count - print number of packets received from ip address
select iface [iface] - select iface for sniffing eth0, wlan0, ethN, wlanN
stat [iface] - show all collected stat for interface, if iface omitted - for all interfaces. 
--help - show usage information
```