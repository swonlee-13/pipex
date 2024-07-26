/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seongwol <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/04 15:41:32 by seongwol          #+#    #+#             */
/*   Updated: 2023/08/17 19:47:43 by seongwol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

int	ft_is_here_doc(char *file)
{
	if (ft_strcmp(file, "here_doc") == 0)
		return (1);
	return (0);
}

void	ft_execute_heredoc(char **av, t_pipex *p)
{
	char	*buffer;
	char	*end;

	p->infile = open("/tmp/sh-thd-1641928925", \
			O_RDWR | O_CREAT | O_EXCL | O_TRUNC, 0644);
	end = ft_strjoin(av[2], "\n");
	while (1)
	{
		write(1, "heredoc> ", 9);
		buffer = get_next_line(STDIN_FILENO);
		if (*buffer == 0 || ft_strcmp(buffer, end) == 0)
			break ;
		ft_putstr_fd(buffer, p->infile);
		free(buffer);
	}
	close(p->infile);
	p->infile = open("/tmp/sh-thd-1641928925", O_RDONLY);
	unlink("/tmp/sh-thd-1641928925");
	free(buffer);
	free(end);
	p->i++;
}
