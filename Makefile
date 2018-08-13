all: rpsls_client rpsls_server

rpsls_client: rpsls_client.c
	gcc -Wall -std=gnu99 -o $@ $^

rpsls_server: rpsls_server.c
	gcc -Wall -std=gnu99 -o $@ $^

clean:
	rm -f rpsls_client rpsls_server
