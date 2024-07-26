/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seongwol <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/04 02:08:57 by seongwol          #+#    #+#             */
/*   Updated: 2023/08/17 19:52:33 by seongwol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_BONUS_H
# define PIPEX_BONUS_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <fcntl.h>
# include "../libft/libft.h"
# include "../libft/get_next_line/get_next_line.h"

# define WRITE 1
# define READ 0

typedef struct s_pipex
{
	pid_t	pid;
	int		fd[2];
	int		infile;
	int		outfile;
	int		i;
}	t_pipex;

typedef struct s_exec
{
	char	**paths;
	char	*path;
	char	**commands;
	char	*copy;
}	t_exec;

int		ft_is_here_doc(char *file);
void	ft_execute_heredoc(char **av, t_pipex *p);
#endif
