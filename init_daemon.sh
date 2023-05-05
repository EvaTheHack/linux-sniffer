cp daemon_init /etc/init.d/daemon_sniffer

OLD_DEAMON_PATH="DEAMON_PATH_VAR"
DEAMON_PATH=$(readlink -f output/daemon_sniffer)

sed -i "s|$OLD_DEAMON_PATH|$DEAMON_PATH|g" /etc/init.d/daemon_sniffer

chmod +x /etc/init.d/daemon_sniffer

update-rc.d daemon_sniffer defaults