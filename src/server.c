/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 11:20:35 by mbatty            #+#    #+#             */
/*   Updated: 2025/11/17 11:25:45 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server_internals.h"

int	server_update(t_server *server)
{
	server_refresh_poll(server);
	int	poll_events = poll(server->fds, MAX_CLIENTS + 1, -1);
	if (poll_events == -1 && errno == EINTR)
		return (1);
	if ((server->fds[0].revents & POLLIN) != 0)
		server_new_client(server);
	server_read_clients(server);
	return (1);
}

int	server_open(t_server *server, int port)
{
	int yes = 1;

	memset(server, 0, sizeof(t_server));
	server->current_client_id = 1;
	server->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server->socket_fd == -1)
		return (0);
	server->server_address.sin_family = AF_INET;
	server->server_address.sin_port = htons(port);
	server->server_address.sin_addr.s_addr = INADDR_ANY;
	if (setsockopt(server->socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		close(server->socket_fd);
		return (0);
	}
	if (bind(server->socket_fd, (struct sockaddr*)&server->server_address, sizeof(server->server_address)) == -1)
	{
		close(server->socket_fd);
		return (0);
	}
	if (listen(server->socket_fd, MAX_CLIENTS) == -1)
	{
		close(server->socket_fd);
		return (0);
	}
	return (1);
}

int	server_close(t_server *server)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
		if (server->clients[i].fd != 0)
			close(server->clients[i].fd);
	if (server->socket_fd != 0)
		close(server->socket_fd);
	return (1);
}

int	server_remove_client(t_server *server, int fd)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (server->clients[i].fd == fd)
		{
			server->clients[i].fd = 0;
			server->clients[i].id = 0;
			return (1);
		}
	}
	return (0);
}

int	server_add_client(t_server *server, int fd)
{
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		if (server->clients[i].fd == 0)
		{
			server->clients[i].fd = fd;
			if (fd != 0)
				server->clients[i].id = server->current_client_id++;
			return (1);
		}
	}
	return (0);
}

int	server_refresh_poll(t_server *server)
{
	server->fds[0].fd = server->socket_fd;
	server->fds[0].events = POLLIN;
	server->fds[0].revents = 0;
	int	i = 1;
	for (int c = 0; c < MAX_CLIENTS; c++)
	{
		server->fds[i].fd = server->clients[c].fd;
		server->fds[i].events = POLLIN;
		server->fds[i].revents = 0;
		i++;
	}
	return (1);
}

int	server_new_client(t_server *server)
{
	struct sockaddr_in		addr;
	unsigned int			len = sizeof(struct sockaddr_in);
	char					ip[INET_ADDRSTRLEN];
	int						fd;
	
	memset(&addr, 0, len);
	fd = accept(server->socket_fd, (struct sockaddr*)&addr, &len);
	if (fd == -1)
		return (0);

	if (!server_add_client(server, 0))
	{
		printf("Too many clients\n");
		close(fd);
		return (1);
	}

	inet_ntop(AF_INET, &addr, ip, INET_ADDRSTRLEN);
	server_add_client(server, fd);

	printf("New client %d\n", fd);

	return (1);
}

int	server_read_clients(t_server *server)
{
	int	i = 1;
	for (int c = 0; c < MAX_CLIENTS;)
	{
		if (server->fds[i].revents & POLLIN)
		{
			char	*msg = NULL;
			char 	buffer[1024];
			ssize_t size;

			while (1)
			{
				size = recv(server->clients[c].fd, buffer, sizeof(buffer), 0);
				if (size == 0 || size == -1)
				{
					printf("Removed client\n");
					server_remove_client(server, server->clients[c].fd);
					goto skip_it;
				}
				buffer[size - 1] = 0;
				msg = server_strjoin(msg, buffer);
				if (size < (ssize_t)sizeof(buffer))
					break ;
			}
			printf("msg: %s\n", msg);
			free(msg);
		}
		
		c++;
	skip_it:
		i++;
	}
	return (1);
}

char	*server_strjoin(char *s1, char *s2)
{
	char	*dest;
	size_t	len;

	if (!s1)
		return (server_strdup(s2));
	len = (strlen(s1) + strlen(s2) + 1);
	dest = malloc(len * sizeof(char));
	if (dest == NULL)
		return (NULL);
	strcpy(dest, s1);
	strcat(dest, s2);
	free(s1);
	return ((char *)dest);
}

char	*server_strdup(const char *s)
{
	int		i;
	char	*dup;

	i = 0;
	dup = malloc((strlen(s) + 1) * sizeof(char));
	if (dup == NULL)
		return (NULL);
	while (s[i] != '\0')
	{
		dup[i] = s[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}
