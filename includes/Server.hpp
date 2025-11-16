/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 14:41:56 by mbatty            #+#    #+#             */
/*   Updated: 2025/11/16 15:52:25 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <arpa/inet.h>
# include <cstring>
# include <iostream>
# include <vector>
# include <deque>
# include <netinet/in.h>
# include <sys/socket.h>
# include <unistd.h>
# include <poll.h>
# include <algorithm>
# include <csignal>
# include <cstdlib>
# include <errno.h>
# include <iomanip>
# include <string>
# include <ctime>
# include <array>
# include <map>

# include "Client.hpp"

class	Server
{
	public:
		static constexpr int	MAX_CLIENTS = 16;
	public:
		Server() {}
		~Server() {}

		void	open(int port);
		void	close();
		void	update();
	private:
		/*
			Sends a message to all clients except the client given in parameters
		*/
		void	_sendAll(Client &client, const std::string &msg)
		{
			for (auto pair : _clients)
			{
				if (client.fd() != pair.second.fd())
					send(pair.second.fd(), msg.c_str(), msg.size(), 0);
			}
		}
		void	_sendAll(const std::string &msg)
		{
			for (auto pair : _clients)
				send(pair.second.fd(), msg.c_str(), msg.size(), 0);
		}
		void	_processInput(Client &client, const std::string &msg);
		void	_refreshPoll();
		void _recvClients();
		void	_addNewClient();
		uint					_curClientID = 0;
		std::map<int, Client>	_clients;

		int						_socketFD = -1;
		sockaddr_in				_serverAddress;
		struct 	pollfd			_fds[Server::MAX_CLIENTS + 1];
};

#endif
