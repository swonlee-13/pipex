/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seongwol <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/28 16:46:24 by seongwol          #+#    #+#             */
/*   Updated: 2023/08/04 14:59:33 by seongwol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <fcntl.h>
# include "../libft/libft.h"

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
#endif
