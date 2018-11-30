all:serv cli

serv: serv.c tcp_listen.c str_echo.c serv.h
	gcc serv.c -pthread -o serv
cli: cli.c tcp_connect.c str_cli.c readline.c
	gcc cli.c -pthread -o cli
clean:
	rm serv cli
