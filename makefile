all: create_folders create_config create_daemon create_cli

create_folders:
	$(shell mkdir -p output)
	$(shell sudo mkdir /results)

create_config:
	touch /config.txt
	echo "eth0" > /config.txt

create_daemon:
	gcc src/daemon.c -o output/daemon_sniffer

create_cli:
	gcc src/cli.c -o output/sniffer
	