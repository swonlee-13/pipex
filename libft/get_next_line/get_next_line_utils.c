/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seongwol <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/18 11:47:52 by seongwol          #+#    #+#             */
/*   Updated: 2023/05/05 17:59:15 by seongwol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

size_t	ft_strlen(const char *str)
{
	size_t	i;

	i = 0;
	if (!str)
		return (0);
	while (*str)
	{
		str++;
		i++;
	}
	return (i);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	s1_len;
	size_t	s2_len;
	size_t	index;
	char	*copy;

	s1_len = ft_strlen(s1);
	s2_len = ft_strlen(s2);
	index = 0;
	copy = malloc(sizeof(char) * (s1_len + s2_len + 1));
	if (!copy)
		return (NULL);
	while (index < s1_len + s2_len + 1)
	{
		copy[index] = 0;
		index++;
	}
	if (s1 != NULL)
		ft_strlcat(copy, s1, s1_len + s2_len + 1);
	ft_strlcat(copy, s2, s1_len + s2_len + 1);
	return (copy);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	index;
	char	*copy;

	index = 0;
	if (len + start > ft_strlen(s))
		len = ft_strlen(s) - start;
	if (start > ft_strlen(s))
		len = 0;
	copy = malloc(len + 1);
	if (!copy)
		return (NULL);
	if (start <= ft_strlen(s))
	{
		while (index < len)
		{
			*(copy + index) = *(s + start + index);
			index++;
		}
	}
	copy[index] = 0;
	return (copy);
}

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	i;
	size_t	srclen;
	size_t	dstlen;

	i = 0;
	if (!src || !dst)
		return (0);
	srclen = ft_strlen(src);
	dstlen = ft_strlen(dst);
	if (dstlen >= dstsize)
		return (dstsize + srclen);
	while (*dst && i + 1 < dstsize)
	{
		i++;
		dst++;
	}
	while (i + 1 < dstsize && *src)
	{
		*dst = *src;
		i++;
		src++;
		dst++;
	}
	*dst = 0;
	return (dstlen + srclen);
}
