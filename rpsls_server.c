#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>    /* Internet domain header */
#include <arpa/inet.h>     /* only needed on my mac */

#define PORT_NUM 60000
#define MAX_BACKLOG 5

void win_lose(int fd1, int fd2) {
    if (write(fd1, "win", 4) == -1) {
        perror("write");
        exit(1);
    }
    if (write(fd2, "lose", 4) == -1) {
        perror("write");
        exit(1);
    }
}

void tie(int fd1, int fd2) {
    if (write(fd1, "tie", 4) == -1) {
        perror("write");
        exit(1);
    }
    if (write(fd2, "tie", 4) == -1) {
        perror("write");
        exit(1);
    }
}

int main(int argc, char **argv) {
    int port1, port2;
    if (argc == 1) {
        port1 = PORT_NUM;
        port2 = PORT_NUM + 1;
    } else if (argc == 2) {
        int n = strtol(argv[1], NULL, 10);
        port1 = PORT_NUM + n;
        port2 = PORT_NUM + 1 + n;
    } else {
        fprintf(stderr, "Usage: rpsls_server [n]\n");
        exit(1);
    }

    // create socket
    int sock_fd1 = socket(AF_INET, SOCK_STREAM, 0);
    int sock_fd2 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd1 == - 1 || sock_fd2 == - 1) {
      perror("server: socket");
      exit(1);
    }

    // Make sure we can reuse the port immediately after the
    // server terminates. Avoids the "address in use" error
    int on = 1;
    if (setsockopt(sock_fd1, SOL_SOCKET, SO_REUSEADDR,
           (const char *) &on, sizeof(on)) == -1) {
        perror("setsockopt");
        exit(1);
    }
    if (setsockopt(sock_fd2, SOL_SOCKET, SO_REUSEADDR,
           (const char *) &on, sizeof(on)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    //initialize server1 address
    struct sockaddr_in server1;
    server1.sin_family = AF_INET;
    server1.sin_port = htons(port1);
    memset(&server1.sin_zero, 0, 8);
    server1.sin_addr.s_addr = INADDR_ANY;
    //initialize server2 address
    struct sockaddr_in server2;
    server2.sin_family = AF_INET;
    server2.sin_port = htons(port2);
    memset(&server2.sin_zero, 0, 8);
    server2.sin_addr.s_addr = INADDR_ANY;

    // bind socket1 to an address
    if (bind(sock_fd1, (struct sockaddr *) &server1, sizeof(struct sockaddr_in)) == -1) {
      perror("server: bind");
      close(sock_fd1);
      exit(1);
    }
    // bind socket2 to an address
    if (bind(sock_fd2, (struct sockaddr *) &server2, sizeof(struct sockaddr_in)) == -1) {
      perror("server: bind");
      close(sock_fd2);
      exit(1);
    }


    // Set up a queue in the kernel to hold pending connections.
    if (listen(sock_fd1, MAX_BACKLOG) < 0) {
        // listen failed
        perror("listen");
        exit(1);
    }
    // Set up a queue in the kernel to hold pending connections.
    if (listen(sock_fd2, MAX_BACKLOG) < 0) {
        // listen failed
        perror("listen");
        exit(1);
    }

    // peer1
    struct sockaddr_in peer1;
    unsigned int peer_len = sizeof(struct sockaddr_in);
    peer1.sin_family = AF_INET;
    // peer2
    struct sockaddr_in peer2;
    peer2.sin_family = AF_INET;

    fprintf(stderr, "Waiting for two connections...\n");
    int client_fd1 = accept(sock_fd1, (struct sockaddr *)&peer1, &peer_len);
    fprintf(stderr, "[%d]First client is connected...\n", client_fd1);
    close(sock_fd1);
    int client_fd2 = accept(sock_fd2, (struct sockaddr *)&peer2, &peer_len);
    fprintf(stderr, "[%d]OK! Here comes the second one.\n", client_fd2);
    if (client_fd1 == -1 || client_fd2 == -1) {
        perror("accept");
        return -1;
    }

    // read their names
    char name1[50];
    char name2[50];
    if (read(client_fd1, name1, 50) == -1) {
        perror("read");
        exit(1);
    }
    if (read(client_fd2, name2, 50) == -1) {
        perror("read");
        exit(1);
    }
    fprintf(stderr, "Username received...\n");

    // broadcast
    if (write(client_fd2, name1, 50) == -1) {
        perror("write");
        exit(1);
    }
    if (write(client_fd1, name2, 50) == -1) {
        perror("write");
        exit(1);
    }

    int stas[3];
    stas[0] = 0; // fd1
    stas[1] = 0;
    stas[2] = 0; // fd2
    char gesture1, gesture2;
    while (1) {
        if (read(client_fd1, &gesture1, 1) == -1) {
            perror("read");
            exit(1);
        }
        if (read(client_fd2, &gesture2, 1) == -1) {
            perror("read");
            exit(1);
        }
        if (gesture1 == 'e') {
            if (write(client_fd1, "end", 4) == -1) {
                perror("write");
                exit(1);
            }
            if (write(client_fd2, "end", 4) == -1) {
                perror("write");
                exit(1);
            }
            break;
        }
        if (gesture2 == 'e') {
            if (write(client_fd1, "end", 4) == -1) {
                perror("write");
                exit(1);
            }
            if (write(client_fd2, "end", 4) == -1) {
                perror("write");
                exit(1);
            }
            break;
        }

        // game logic
        switch (gesture1) {
            case 'r':
                switch (gesture2) {
                    case 'r':
                        tie(client_fd1, client_fd2);
                        stas[1] ++;
                        break;
                    case 'p':
                        win_lose(client_fd2, client_fd1);
                        stas[2] ++;
                        break;
                    case 's':
                        win_lose(client_fd1, client_fd2);
                        stas[0] ++;
                        break;
                    case 'l':
                        win_lose(client_fd1, client_fd2);
                        stas[0] ++;
                        break;
                    case 'S':
                        win_lose(client_fd2, client_fd1);
                        stas[2] ++;
                        break;
                }
                break;
            case 'p':
                switch (gesture2) {
                    case 'r':
                        win_lose(client_fd1, client_fd2);
                        stas[0] ++;
                        break;
                    case 'p':
                        tie(client_fd1, client_fd2);
                        stas[1] ++;
                        break;
                    case 's':
                        win_lose(client_fd2, client_fd1);
                        stas[2] ++;
                        break;
                    case 'l':
                        win_lose(client_fd2, client_fd1);
                        stas[2] ++;
                        break;
                    case 'S':
                        win_lose(client_fd1, client_fd2);
                        stas[0] ++;
                        break;
                }
                break;
            case 's':
                switch (gesture2) {
                    case 'r':
                        win_lose(client_fd2, client_fd1);
                        stas[2] ++;
                        break;
                    case 'p':
                        win_lose(client_fd1, client_fd2);
                        stas[0] ++;
                        break;
                    case 's':
                        tie(client_fd1, client_fd2);
                        stas[1] ++;
                        break;
                    case 'l':
                        win_lose(client_fd1, client_fd2);
                        stas[0] ++;
                        break;
                    case 'S':
                        win_lose(client_fd2, client_fd1);
                        stas[2] ++;
                        break;
                }
                break;
            case 'l':
                switch (gesture2) {
                    case 'r':
                        win_lose(client_fd2, client_fd1);
                        stas[2] ++;
                        break;
                    case 'p':
                        win_lose(client_fd1, client_fd2);
                        stas[0] ++;
                        break;
                    case 's':
                        win_lose(client_fd2, client_fd1);
                        stas[2] ++;
                        break;
                    case 'l':
                        tie(client_fd1, client_fd2);
                        stas[1] ++;
                        break;
                    case 'S':
                        win_lose(client_fd1, client_fd2);
                        stas[0] ++;
                        break;
                }
                break;
            case 'S':
                switch (gesture2) {
                    case 'r':
                        win_lose(client_fd1, client_fd2);
                        stas[0] ++;
                        break;
                    case 'p':
                        win_lose(client_fd2, client_fd1);
                        stas[2] ++;
                        break;
                    case 's':
                        win_lose(client_fd1, client_fd2);
                        stas[0] ++;
                        break;
                    case 'l':
                        win_lose(client_fd2, client_fd1);
                        stas[2] ++;
                        break;
                    case 'S':
                        tie(client_fd1, client_fd2);
                        stas[1] ++;
                        break;
                }
                break;
        }

    }

    char stas_mesg1[50];
    char stas_mesg2[50];
    sprintf(stas_mesg1, "win: %d | tie: %d | lose: %d\nTotal:%d games\n",
            stas[0], stas[1], stas[2], stas[0]+stas[1]+stas[2]);
    sprintf(stas_mesg2, "win: %d | tie: %d | lose: %d\nTotal:%d games\n",
            stas[2], stas[1], stas[0], stas[0]+stas[1]+stas[2]);

    if (write(client_fd1, stas_mesg1, 50) == -1) {
        perror("write");
        exit(1);
    }
    if (write(client_fd2, stas_mesg2, 50) == -1) {
        perror("write");
        exit(1);
    }

    close(client_fd1);
    close(client_fd2);
    close(sock_fd1);
    close(sock_fd2);

    return 0;
}
