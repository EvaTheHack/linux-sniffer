all: create_folders create_config create_result_path create_daemon create_cli

create_folders:
	$(shell mkdir -p output)
	$(shell mkdir -p results)

create_config:
	touch config.txt
	echo "eth0" > config.txt

create_result_path:
	touch results_path.txt
	echo "$(CURDIR)/results" > results_path.txt

create_daemon:
	gcc src/daemon.c -o output/daemon_sniffer

create_cli:
	gcc src/cli.c -o output/sniffer
	