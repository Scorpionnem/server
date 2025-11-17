/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 14:41:16 by mbatty            #+#    #+#             */
/*   Updated: 2025/11/17 11:31:11 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iostream>
#include <cstdlib>
#include <sys/signal.h>

int	g_sig = 0;

void	handleSig(int sig)
{
	g_sig = sig;
}

int	main(void)
{
	signal(SIGINT, handleSig);

	Server	server;
	server.setConnectCallback([]
		(const Client &client)
		{
			std::cout << "Connect callback for client: " << client.id() << std::endl;
		});
	server.setDisconnectCallback([]
		(const Client &client)
		{
			std::cout << "Disconnect callback for client: " << client.id() << std::endl;
		});
	server.setMessageCallback([&server]
		(const Client &client, const std::string &msg)
		{
			server.sendAll(client, msg);
			std::cout << "Message callback for client: " << client.id() << " : " << msg << std::flush;
		});

	try {
		server.open(6942);
		while (g_sig == 0)
			server.update();
		server.close();
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
}
