/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 15:41:25 by mbatty            #+#    #+#             */
/*   Updated: 2025/11/16 17:18:08 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <cstdlib>

class	Client
{
	public:
		Client(uint id, uint fd)
		{
			_id = id;
			_fd = fd;
		}
		~Client() {}

		uint	id() const {return (_id);}
		uint	fd() const {return (_fd);}
	private:
		uint		_id;
		uint		_fd;
};

#endif
