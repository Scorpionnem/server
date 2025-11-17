/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 18:15:07 by mbatty            #+#    #+#             */
/*   Updated: 2025/11/17 11:25:33 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
# define SERVER_H

# include <arpa/inet.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <unistd.h>
# include <stdlib.h>
# include <poll.h>
# include <string.h>
# include <errno.h>
# include <stdio.h>

# define MAX_CLIENTS 2
#  if MAX_CLIENTS <= 0
#   error "MAX_CLIENTS should be > 0"
#  endif

typedef struct s_client
{
	int	fd;
	int	id;
}	t_client;

typedef struct s_server
{
	int					socket_fd;
	struct sockaddr_in	server_address;
	struct 	pollfd		fds[MAX_CLIENTS + 1];

	unsigned int		current_client_id;
	t_client			clients[MAX_CLIENTS + 1];
}	t_server;

int	server_update(t_server *server);
int	server_close(t_server *server);
int	server_open(t_server *server, int port);

#endif
