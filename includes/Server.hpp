/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 14:41:56 by mbatty            #+#    #+#             */
/*   Updated: 2025/11/16 17:18:28 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <arpa/inet.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <unistd.h>
# include <poll.h>
# include <string>
# include <map>
# include <functional>
# include <stdexcept>
# include <cstring>

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

		void	setConnectCallback(std::function<void(const Client &)> func) {_onConnect = func;}
		void	setDisconnectCallback(std::function<void(const Client &)> func) {_onDisconnect = func;}
		void	setMessageCallback(std::function<void(const Client &, const std::string &)> func) {_onMessage = func;}
		/*
			Sends a message to all clients except the client given in parameters
		*/
		void	sendAll(Client &client, const std::string &msg)
		{
			for (auto pair : _clients)
			{
				if (client.fd() != pair.second.fd())
					send(pair.second.fd(), msg.c_str(), msg.size(), 0);
			}
		}
		void	sendAll(const std::string &msg)
		{
			for (auto pair : _clients)
				send(pair.second.fd(), msg.c_str(), msg.size(), 0);
		}
	private:
		void	_processInput(Client &client, const std::string &msg);
		void	_refreshPoll();
		void 	_recvClients();
		void	_addNewClient();
		uint					_curClientID = 0;
		std::map<int, Client>	_clients;

		int						_socketFD = -1;
		sockaddr_in				_serverAddress;
		struct 	pollfd			_fds[Server::MAX_CLIENTS + 1];

		std::function<void(const Client &)>						_onConnect = NULL;
		std::function<void(const Client &)>						_onDisconnect = NULL;
		std::function<void(const Client &, const std::string &)>	_onMessage = NULL;
};

#endif
