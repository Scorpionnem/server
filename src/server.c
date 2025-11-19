/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 11:20:35 by mbatty            #+#    #+#             */
/*   Updated: 2025/11/19 15:11:44 by mbatty           ###   ########.fr       */
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
	server->clients = calloc(0, sizeof(t_client));
	return (1);
}

int	server_close(t_server *server)
{
	for (int i = 0; i < server->clients_count; i++)
		if (server->clients[i].fd != 0)
			close(server->clients[i].fd);
	if (server->socket_fd != 0)
		close(server->socket_fd);
	free(server->clients);
	return (1);
}

void	server_set_message_hook(t_server *server, void (*func)(t_client *client, char *msg, void *arg), void *arg)
{
	server->message_hook = func;
	server->message_hook_arg = arg;
}

void	server_set_disconnect_hook(t_server *server, void (*func)(t_client *client, void *arg), void *arg)
{
	server->disconnect_hook = func;
	server->disconnect_hook_arg = arg;
}

void	server_set_connect_hook(t_server *server, void (*func)(t_client *client, void *arg), void *arg)
{
	server->connect_hook = func;
	server->connect_hook_arg = arg;
}

int	server_send_to_fd(int fd, const char *msg)
{
	send(fd, msg, strlen(msg), 0);
	return (1);
}

int	server_send_to_id(t_server *server, int id, const char *msg)
{
	for (int c = 0; c < server->clients_count; c++)
		if (server->clients[c].id == id)
		{
			send(server->clients[c].fd, msg, strlen(msg), 0);
			break ;
		}
	return (1);
}

int	server_remove_client(t_server *server, int fd)
{
	t_client	*tmp = calloc(server->clients_count - 1, sizeof(t_client));

	int	i = 0;
	for (int c = 0; c < server->clients_count; c++)
	{
		if (server->clients[c].fd != fd)
		{
			tmp[i].fd = server->clients[c].fd;
			tmp[i].id = server->clients[c].id;
			i++;
		}
	}
	free(server->clients);
	server->clients = tmp;
	server->clients_count--;
	return (1);
}

int	server_add_client(t_server *server, int fd)
{
	server->clients = server_realloc(server->clients, server->clients_count * sizeof(t_client), sizeof(t_client));
	server->clients[server->clients_count].fd = fd;
	server->clients[server->clients_count].id = server->current_client_id++;
	server->clients_count++;
	return (server->clients_count - 1);
}

int	server_refresh_poll(t_server *server)
{
	server->fds[0].fd = server->socket_fd;
	server->fds[0].events = POLLIN;
	server->fds[0].revents = 0;
	int	i = 1;
	for (int c = 0; c < server->clients_count; c++)
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

	inet_ntop(AF_INET, &addr, ip, INET_ADDRSTRLEN);
	int client = server_add_client(server, fd);

	if (server->connect_hook)
		server->connect_hook(&server->clients[client], server->connect_hook_arg);
	return (1);
}

int	server_read_clients(t_server *server)
{
	int	i = 1;
	for (int c = 0; c < server->clients_count;)
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
					if (server->disconnect_hook)
						server->disconnect_hook(&server->clients[c], server->disconnect_hook_arg);
					server_remove_client(server, server->clients[c].fd);
					goto skip_it;
				}
				buffer[size - 1] = 0;
				msg = server_strjoin(msg, buffer);
				if (size < (ssize_t)sizeof(buffer))
					break ;
			}
			if (server->message_hook)
				server->message_hook(&server->clients[c], msg, server->message_hook_arg);
			free(msg);
		}
		
		c++;
	skip_it:
		i++;
	}
	return (1);
}
