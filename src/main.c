/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 18:15:16 by mbatty            #+#    #+#             */
/*   Updated: 2025/11/17 17:51:36 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.h"
#include "sys/signal.h"

int	g_sig = 0;

void	handle_sig(int sig)
{
	g_sig = sig;
}

void	message_hook(t_client *client, char *msg, void *ptr)
{
	// t_server	*server = ptr;
	
	// (void)server;
	(void)client;(void)ptr;(void)msg;
	// if (!strcmp(msg, "shell"))
	// {
	// 	if (fork() == 0)
	// 	{
	// 		// dup2(client->fd, STDOUT_FILENO);
	// 		// dup2(client->fd, STDERR_FILENO);
	// 		dup2(client->fd, STDIN_FILENO);
	// 		char	**av = {NULL}; 
	// 		execv("/bin/sh", av);
	// 		exit(EXIT_SUCCESS);
	// 	}
	// 	server_remove_client(server, client->fd);
	// }
	// else
		printf("From %d: %s\n", client->id, msg);
}

void	connect_hook(t_client *client, void *ptr)
{
	(void)client;(void)ptr;
	printf("Client %d joined\n", client->id);
}

void	disconnect_hook(t_client *client, void *ptr)
{
	(void)client;(void)ptr;
	printf("Client %d left\n", client->id);
}

int	main(void)
{
	t_server	server;

	signal(SIGINT, handle_sig);
	server_open(&server, 7002);
	server_set_message_hook(&server, message_hook, &server);
	server_set_connect_hook(&server, connect_hook, &server);
	server_set_disconnect_hook(&server, disconnect_hook, &server);
	while (g_sig == 0)
		server_update(&server);
	server_close(&server);
	return (0);
}
