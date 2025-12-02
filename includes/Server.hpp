/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 14:41:56 by mbatty            #+#    #+#             */
/*   Updated: 2025/12/03 00:12:30 by mbatty           ###   ########.fr       */
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
# include <iostream>

class	Server
{
	public:
		static constexpr int	MAX_CLIENTS = 16;

		class	Client
		{
			public:
				Client(uint fd)
				{
					_fd = fd;
				}
				~Client() {}
			
				uint	fd() const {return (_fd);}
				std::string	getBuffer() {return (_buffer);}
				void	setBuffer(const std::string buf) {_buffer = buf;}
			private:
				std::string	_buffer;
				uint		_fd;
		};
	public:
		Server() {}
		~Server() {}

		void	open(int port);
		void	close();
		void	update();

		void	setConnectCallback(std::function<void(const Server::Client &)> func) {_onConnect = func;}
		void	setDisconnectCallback(std::function<void(const Server::Client &)> func) {_onDisconnect = func;}
		void	setMessageCallback(std::function<void(const Server::Client &, const std::string &)> func) {_onMessage = func;}

		/*
			Sends a message to all clients except the client given in parameters

			@throws runtime_error if send fails
		*/
		void	sendAll(const Server::Client &client, const std::string &msg) const
		{
			for (auto pair : _clients)
			{
				if (client.fd() != pair.second.fd())
					if (send(pair.second.fd(), msg.c_str(), msg.size(), 0) == -1)
						throw std::runtime_error("Failed to send message to fd " + std::to_string(pair.second.fd()));
			}
		}
		/*
			@throws runtime_error if send fails
		*/
		void	sendAll(const std::string &msg) const
		{
			for (auto pair : _clients)
				if (send(pair.second.fd(), msg.c_str(), msg.size(), 0) == -1)
					throw std::runtime_error("Failed to send message to fd " + std::to_string(pair.second.fd()));
		}
		/*
			@throws runtime_error if send fails
		*/
		void	sendClient(const Server::Client &client, const std::string &msg) const
		{
			if (send(client.fd(), msg.c_str(), msg.size(), 0) == -1)
				throw std::runtime_error("Failed to send message to fd " + std::to_string(client.fd()));
		}
	private:
		void		_processInput(Server::Client &client, const std::string &msg);
		void		_refreshPoll();
		void 		_recvClients();
		void		_addNewClient();
		std::map<int, Server::Client>	_clients;

		int						_socketFD = -1;
		sockaddr_in				_serverAddress;
		struct 	pollfd			_fds[Server::MAX_CLIENTS + 1];

		std::function<void(const Server::Client &)>							_onConnect = NULL;
		std::function<void(const Server::Client &)>							_onDisconnect = NULL;
		std::function<void(const Server::Client &, const std::string &)>	_onMessage = NULL;
};

#endif
