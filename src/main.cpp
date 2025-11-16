/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 14:41:16 by mbatty            #+#    #+#             */
/*   Updated: 2025/11/16 15:55:43 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int	g_sig = 0;

void	handleSig(int sig)
{
	g_sig = sig;
}

int	main(void)
{
	signal(SIGINT, handleSig);

	Server	server;
	server.open(4242);
	while (g_sig == 0)
		server.update();
	server.close();
}
