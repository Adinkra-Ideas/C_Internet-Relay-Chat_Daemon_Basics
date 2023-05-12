#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>

// Max value for a localhost port number == max 16 bit int value (65535)
# define MAX_PORT_VAL 65535

// The port numbers below 1024 are reserved ports
# define MIN_PORT_VAL 1024

// max buffer for storing data received from client
# define MAX_READ_LEN 10000

// max strlen inserted into g_bufferWrite by sprintf()
# define MAX_WRITE_LEN 200

	int     			g_client_id[FD_SETSIZE + 2];	// Array with [Key]=>Value mapped as [FD_Assigned_To_Client] => Chat_ID_Assigned_To_Client
	fd_set      		g_readfds, g_writefds, g_active;
	int        			g_biggest_fd_val = 0;
	unsigned long long	g_client_ID_Assignor = 0;
	char        		g_bufferRead[MAX_READ_LEN + 2], g_bufferWrite[MAX_WRITE_LEN];	// g_bufferRead for storing data received from clients. g_bufferWrite for storing data to be sent to clients

void    broadcast_chat( int exclude_me ) {
    for( int i = 0; i <= g_biggest_fd_val; i++ )
        if( FD_ISSET(i, &g_writefds) && i != exclude_me )
            write(i, g_bufferWrite, strlen(g_bufferWrite));
}

void	exit_with_error( char *str )
{
	write(2, str, strlen(str));
	exit(1);
}

int	main(int argc, char **av)
{
	int						_server_fd = 0;
	struct sockaddr_in		_sockAddr;
	socklen_t 				_sockAddr_len = sizeof(_sockAddr);
	int						_port = (argc > 1) ? atoi(av[1]) : 0;
	if ( argc <= 1 )
		exit_with_error("Wrong number of arguments\n");

	bzero(&_sockAddr, _sockAddr_len);
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_port = htons(_port);
	_sockAddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1

	if ( (_port > MAX_PORT_VAL)
		|| (_port < MIN_PORT_VAL)
		|| ((_server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		|| (bind(_server_fd, (struct sockaddr *)&_sockAddr, _sockAddr_len) < 0)
		|| (listen(_server_fd, 0)) ) {
		if ( _server_fd != -1 )
			close(_server_fd);
		exit_with_error("Fatal error\n");
	}

	FD_ZERO(&g_active);
	FD_SET(_server_fd, &g_active);
    g_biggest_fd_val = _server_fd;

	while( 1 ) {
        g_readfds = g_writefds = g_active;
        if ( select(g_biggest_fd_val + 1, &g_readfds, &g_writefds, NULL, NULL) < 0 )
            continue;
        for( int client_FD = 0; client_FD <= g_biggest_fd_val; client_FD++ ) {
			// managing newly-connected client
            if ( FD_ISSET(client_FD, &g_readfds) && client_FD == _server_fd ) {
                int new_client_fd = accept(_server_fd, (struct sockaddr *)&_sockAddr, &_sockAddr_len);
                if ( new_client_fd < 0 )
                    continue;
				if ( new_client_fd >= FD_SETSIZE ) {
					close(new_client_fd);
					continue;
				}
                g_biggest_fd_val = new_client_fd > g_biggest_fd_val ? new_client_fd : g_biggest_fd_val;
                g_client_id[new_client_fd] = g_client_ID_Assignor++;
                FD_SET(new_client_fd, &g_active);
                sprintf(g_bufferWrite, "server: client %d just arrived\n", g_client_id[new_client_fd]);
                broadcast_chat(new_client_fd);
                break;
            }

			// Managing existing clients chats
            if ( FD_ISSET(client_FD, &g_readfds) && client_FD != _server_fd ) {
				// The MSG_WAITALL should make recv() block until all chunks are received from Client
                ssize_t recvd_msg_len = recv(client_FD, g_bufferRead, MAX_READ_LEN, 0); // Added MSG_WAITALL Flag to recv()

                if ( recvd_msg_len <= 0 ) {
                    sprintf(g_bufferWrite, "server: client %d just left\n", g_client_id[client_FD]);
                    broadcast_chat(client_FD);
                    FD_CLR(client_FD, &g_active);
                    close(client_FD);
					g_biggest_fd_val = (client_FD == g_biggest_fd_val) ? g_biggest_fd_val - 1 : g_biggest_fd_val;
					break;
                }
                else {
					size_t	is_multi_line = 0;

					g_bufferRead[recvd_msg_len] = '\0';
					sprintf(g_bufferWrite, "client %d: ", g_client_id[client_FD]);
					broadcast_chat(client_FD);

					// broadcast chat to all clients one char at a time
                    for ( ssize_t i = 0; i < recvd_msg_len; i++ ) {
						// multi line flag operation to balance output for single line and multi-line messages
						if ( i + 1 == recvd_msg_len && !is_multi_line && g_bufferRead[i] == '\n')    
							break ;
						else if ( g_bufferRead[i] == '\n' )
							is_multi_line++;

						if ( g_bufferRead[i] != '\n' ) {
							sprintf(g_bufferWrite, "%c", g_bufferRead[i]);
                        	broadcast_chat(client_FD);
						}

						// print new client X: message on new line if encountered '\n' in a multiline chat
						if ( g_bufferRead[i] == '\n' && i + 1 != recvd_msg_len ) {
							sprintf(g_bufferWrite, "\nclient %d: ", g_client_id[client_FD]);
							broadcast_chat(client_FD);
						}
                    }
					sprintf(g_bufferWrite, "%c", '\n');
					broadcast_chat(client_FD);
					break;
                }
            }
        }
    }

	close(_server_fd);

	return 0;
}
