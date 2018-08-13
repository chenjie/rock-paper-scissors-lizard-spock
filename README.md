# rock-paper-scissors-lizard-Spock
A C socket client-server implementation of Rock Paper Scissors Lizard Spock game.

![img](http://www.samkass.com/theories/RPSSL.gif)

## Getting Started

### Prerequisites

* GCC
* UNIX Shell OR Windows PowerShell

### Download source code and compile
The following instructions are presented using -bash in macOS:
```
# Change to HOME directory
$ cd ~

# Clone this repo and 'cd' into it
$ git clone https://github.com/jellycsc/rock-paper-scissors-lizard-Spock.git
$ cd rock-paper-scissors-lizard-Spock/

# Let's compile.
$ make
gcc -Wall -std=gnu99 -o rpsls_client rpsls_client.c
gcc -Wall -std=gnu99 -o rpsls_server rpsls_server.c
```

### Usage
Server: (default port number is 60000)
```
USAGE: rpsls_server [port_num_offset]
```

Client: (default port number is 60000)
```
USAGE: rpsls_client server_ip [port_num_offset]
```

### Example
Server:
```
$ ./rpsls_server 
Waiting for two connections...
[5]First client is connected...
[3]OK! Here comes the second one.
Username received...
```

Client1:
```
$ ./rpsls_client localhost
Please enter your username:
Jack
Your op's name is Ben
Please enter your gesture:
r
Lose.TAT
Please enter your gesture:
d
Bad gesture! Re-enter please:
S
Ending...
win: 0 | tie: 0 | lose: 1
Total:1 games
```

Client2:
```
$ ./rpsls_client localhost 1
Please enter your username:
Ben
Your op's name is Jack
Please enter your gesture:
p
WIN!!!
Please enter your gesture:
e
Ending...
win: 1 | tie: 0 | lose: 0
Total:1 games
```

## Authors

| Name                    | GitHub                                     | Email
| ----------------------- | ------------------------------------------ | -------------------------
| Chenjie (Jack) Ni       | [jellycsc](https://github.com/jellycsc)    | nichenjie2013@gmail.com

## Thoughts and future improvements

* Details of the "Rock Paper Scissors Lizard Spock" game can be found [here](http://bigbangtheory.wikia.com/wiki/Rock_Paper_Scissors_Lizard_Spock).

## Contributing to this project

1. Fork it [![GitHub forks](https://img.shields.io/github/forks/jellycsc/rock-paper-scissors-lizard-Spock.svg?style=social&label=Fork&maxAge=2592000)](https://github.com/jellycsc/rock-paper-scissors-lizard-Spock/fork)
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -m 'Add some feature'`)
4. Push to your feature branch (`git push origin my-new-feature`)
5. Create a new Pull Request

Details are described [here](https://git-scm.com/book/en/v2/GitHub-Contributing-to-a-Project).

## Bug Reporting [![GitHub issues](https://img.shields.io/github/issues/jellycsc/rock-paper-scissors-lizard-Spock.svg)](https://github.com/jellycsc/rock-paper-scissors-lizard-Spock/issues/)

Please click `issue` button above↑ to report any issues related to this project  
OR you can shoot an email to <nichenjie2013@gmail.com>

## License
This project is licensed under GNU General Public License v3.0 - see [LICENSE](LICENSE) file for more details.
