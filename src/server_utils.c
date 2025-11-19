/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/17 12:26:03 by mbatty            #+#    #+#             */
/*   Updated: 2025/11/19 14:38:09 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server_internals.h"

void	*server_realloc(void *ptr, ssize_t ptr_size, ssize_t add_size)
{
	void	*new;
	int		min;

	if (ptr_size < 0 || ptr_size + add_size < 0)
		return (0);
	new = calloc(ptr_size + add_size, 1);
	if (!new)
		return (0);
	if (ptr_size != 0)
	{
		min = ptr_size;
		if (add_size < 0)
			min = ptr_size + add_size;
		memcpy(new, ptr, min);
	}
	free(ptr);
	return (new);
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
