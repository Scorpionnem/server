/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 14:41:22 by mbatty            #+#    #+#             */
/*   Updated: 2025/12/03 00:15:21 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::_processInput(Server::Client &client, const std::string &msg)
{
	if (_onMessage)
		_onMessage(client, msg);
}

void	Server::update()
{
	_refreshPoll();
	int	pollEvents = poll(_fds, _clients.size() + 1, -1);
	if (pollEvents == -1 && errno == EINTR)
		return ;

	if ((_fds[0].revents & POLLIN) != 0)
	{
		_addNewClient();
		return ;
	}

	_recvClients();
}

void Server::_recvClients()
{
	int i = 1;

	for (auto it = _clients.begin(); it != _clients.end();)
	{
		Server::Client	&client = it->second;

		if (_fds[i].revents & POLLIN)
		{
			std::string msg = client.getBuffer();
			char buffer[1024];
			ssize_t size;

			while (true)
			{
				size = recv(client.fd(), buffer, sizeof(buffer), 0);
				if (size == 0 || size == -1)
				{
					if (_onDisconnect)
						_onDisconnect(client);
					it = _clients.erase(it);
					goto skip_it;
				}
				msg.append(buffer, size);
				if (msg.find('\n') != msg.npos)
					break ;
			}
			while (true)
			{
				if (msg.find('\n') == msg.npos)
					break ;
				_processInput(client, msg.substr(0, msg.find('\n')));
				msg = msg.substr(msg.find('\n') + 1);
			}
			client.setBuffer(msg);
		}

		++it;
	skip_it:
		++i;
	}
}

void	Server::_addNewClient()
{
	sockaddr_in		addr;
	unsigned int	len = sizeof(sockaddr_in);
	char			ip[INET_ADDRSTRLEN];
	int				clientFD;

	std::memset(&addr, 0, sizeof(sockaddr_in));
	clientFD = accept(_socketFD, (struct sockaddr*)&addr, &len);
	if (clientFD == -1)
		return ;

	if (_clients.size() >= Server::MAX_CLIENTS)
	{
		::close(clientFD);
		return ;
	}

	inet_ntop(AF_INET, &addr, ip, INET_ADDRSTRLEN);

	Server::Client	client(clientFD);
	_clients.insert({clientFD, client});

	if (_onConnect)
		_onConnect(client);
}

void	Server::_refreshPoll()
{
	_fds[0].fd = _socketFD;
	_fds[0].events = POLLIN;
	_fds[0].revents = 0;
	int	i = 1;
	for (auto pair : _clients)
	{
		Server::Client	&client = pair.second;
		_fds[i].fd = client.fd();
		_fds[i].events = POLLIN;
		_fds[i].revents = 0;
		i++;
	}
}

void	Server::open(int port)
{
	_socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFD == -1)
		throw std::runtime_error(strerror(errno));
	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_port = htons(port);
	_serverAddress.sin_addr.s_addr = INADDR_ANY;

	int yes = 1;
	if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
	{
		::close(_socketFD);
		throw std::runtime_error(strerror(errno));
	}
	if (bind(this->_socketFD, (struct sockaddr*)&this->_serverAddress, sizeof(this->_serverAddress)) == -1)
	{
		::close(_socketFD);
		throw std::runtime_error(strerror(errno));
	}
	if (listen(_socketFD, MAX_CLIENTS) == -1)
	{
		::close(_socketFD);
		throw std::runtime_error(strerror(errno));
	}
}

void	Server::close()
{
	for (auto pair : _clients)
		::close(pair.first);
	if (_socketFD != -1)
		::close(_socketFD);
}
