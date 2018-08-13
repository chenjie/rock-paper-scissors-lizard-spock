#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>    /* Internet domain header */
#include <arpa/inet.h>     /* only needed on my mac */
#include <netdb.h>         /* gethostname */

#define PORT_NUM 60000

int main(int argc, char **argv) {
    int port;
    if (argc == 2) {
        port = PORT_NUM;
    } else if (argc == 3) {
        int n = strtol(argv[2], NULL, 10);
        port = PORT_NUM + n;
    } else {
        fprintf(stderr, "Usage: rpsls_client server_ip [n]\n");
        exit(1);
    }
    char *hostname = argv[1];

    int soc = socket(AF_INET, SOCK_STREAM, 0);
    if (soc < 0) {
        perror("socket");
        exit(1);
    }
    struct sockaddr_in addr;
    // Allow sockets across machines.
    addr.sin_family = AF_INET;
    // The port the server will be listening on.
    addr.sin_port = htons(port);
    // Clear this field; sin_zero is used for padding for the struct.
    memset(&(addr.sin_zero), 0, 8);

    // Lookup host IP address.
    struct hostent *hp = gethostbyname(hostname);
    if (hp == NULL) {
        fprintf(stderr, "unknown host %s\n", hostname);
        exit(1);
    }

    addr.sin_addr = *((struct in_addr *) hp->h_addr);

    // Request connection to server.
    if (connect(soc, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        port += 1;
        addr.sin_port = htons(port);
        if (connect(soc, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
            perror("connect");
            exit(1);
        }
    }

    char name[50];
    char op_name[50];
    printf("Please enter your username:\n");
    scanf("%s", name);

    // write name
    if (write(soc, name, 50) == -1) {
        perror("write");
        exit(1);
    }

    // read opponent's name
    if (read(soc, op_name, 50) == -1) {
        perror("read");
        exit(1);
    }
    printf("Your op's name is %s\n", op_name);

    char mesg[5];
    do {
        char gesture;
        printf("Please enter your gesture:\n");
        scanf(" %c", &gesture);
        while (gesture != 'r' && gesture != 'p' && gesture != 's' && gesture != 'l' && gesture != 'S' && gesture != 'e') {
            printf("Bad gesture! Re-enter please:\n");
            scanf(" %c", &gesture);
        }
        if (write(soc, &gesture, 1) == -1) {
            perror("write");
            exit(1);
        }
        if (gesture == 'e') {
            fprintf(stderr, "Ending...\n");
            read(soc, mesg, 5);
            break;
        }
        read(soc, mesg, 5);
        if (strcmp("win", mesg) == 0) {
            printf("WIN!!!\n");
        } else if (strcmp("lose", mesg) == 0) {
            printf("Lose.TAT\n");
        } else if (strcmp("tie", mesg) == 0) {
            printf("Tie.\n");
        } else {
            fprintf(stderr, "Ending...\n");
            break;
        }
    } while (1);

    // statistic
    char stas_mesg[50];
    if (read(soc, stas_mesg, 50) == -1) {
        perror("read");
        exit(1);
    }
    printf("%s\n", stas_mesg);
    close(soc);
    return 0;
}
