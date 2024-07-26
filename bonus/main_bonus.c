/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: seongwol <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/04 14:21:12 by seongwol          #+#    #+#             */
/*   Updated: 2023/08/17 20:24:02 by seongwol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./pipex_bonus.h"

char	*find_path_from_env(char **env)
{
	int	i;

	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], "PATH=", 5) == 0)
			return (env[i] + 5);
		i++;
	}
	if (env[i] == NULL)
		ft_error("FATAL ERROR - environment variable PATH missing");
	return (NULL);
}

void	ft_execute(char *command, char **env)
{
	t_exec	s;
	int		i;

	i = -1;
	s.path = find_path_from_env(env);
	s.paths = ft_split(s.path, ':');
	s.commands = ft_split(command, ' ');
	while (s.paths[++i])
	{
		s.path = ft_strjoin(s.paths[i], "/");
		s.copy = ft_strjoin(s.path, s.commands[0]);
		if (!s.copy)
			ft_error("command error\n");
		free(s.path);
		if (access(s.copy, X_OK) == 0)
		{
			free(s.commands[0]);
			s.commands[0] = s.copy;
			ft_free(s.paths, ft_strslen(s.paths));
			execve(s.copy, s.commands, env);
		}
		free(s.copy);
	}
	ft_error("failed to execute\n");
}

void	child_process(int ac, char **av, char **env, t_pipex *p)
{
	if (p->infile == -1)
		ft_error("input | output error\n");
	dup2(p->infile, STDIN_FILENO);
	if (p->i == ac - 2)
		dup2(p->outfile, STDOUT_FILENO);
	else
	{
		dup2(p->fd[WRITE], STDOUT_FILENO);
		close(p->outfile);
	}
	close(p->fd[WRITE]);
	close(p->fd[READ]);
	close(p->infile);
	ft_execute(av[p->i], env);
}

void	process_action(int ac, char **av, char **env, t_pipex *p)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
		ft_error("fork() error - failed to create new process\n");
	else if (pid == 0)
		child_process(ac, av, env, p);
	else
	{
		close(p->infile);
		p->infile = dup(p->fd[READ]);
		close(p->fd[WRITE]);
		close(p->fd[READ]);
	}
}

int	main(int ac, char **av, char **env)
{
	t_pipex	p; 

	if ((ft_strcmp(av[1], "here_doc") == 0 && ac < 6) || ac < 5)
		ft_error("arguments error");
	p.i = 2;
	if (ft_is_here_doc(av[1]))
	{
		p.outfile = open(av[ac - 1], O_RDWR | O_CREAT | O_APPEND, 0644);
		ft_execute_heredoc(av, &p);
	}
	else
	{
		p.infile = open(av[1], O_RDONLY);
		p.outfile = open(av[ac - 1], O_RDWR | O_CREAT | O_TRUNC, 0644);
	}
	while (p.i <= ac - 2)
	{
		pipe(p.fd);
		process_action(ac, av, env, &p);
		p.i++;
	}
	while (waitpid(0, NULL, 0) >= 0)
		;
	close(p.infile);
	close(p.outfile);
}
