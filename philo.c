/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: admaupie <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/17 18:02:21 by admaupie          #+#    #+#             */
/*   Updated: 2022/10/31 15:45:41 by admaupie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "philo.h"
#include <sys/time.h>

int				g_dead = 0;
pthread_mutex_t	g_mut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t	g_wri = PTHREAD_MUTEX_INITIALIZER;


long long time_diff(struct timeval *start, struct timeval end)
{
    return ((long long)(end.tv_sec - start->tv_sec) * 1000 + (end.tv_usec - start->tv_usec) / 1000);
}

struct timeval timenow(void)
{
    struct timeval now;

    gettimeofday(&now, NULL);
    return (now);
}

int	should_die(struct timeval feed, int time_to_die)
{
	if (time_diff(&feed, timenow()) >= time_to_die)
		return (1);
	return (0);
}
int		dead_mate(void)
{
	int	ret;

	pthread_mutex_lock(&g_mut);
	ret = g_dead;
	pthread_mutex_unlock(&g_mut);
	return (ret);
}
void	print_mess(t_philo *datas, int num)
{
	long long int	time;

	pthread_mutex_lock(&g_wri);
	time = time_diff(&datas->start, timenow());
	if (num == 1 && !dead_mate())
		printf("%lld %d has taken a fork\n", time, datas->id);
	else if (num == 2 && !dead_mate())
		printf("%lld %d is eating\n", time, datas->id);
	else if (num == 3 && !dead_mate())
		printf("%lld %d is sleeping\n", time, datas->id);
	else if (num == 4 && !dead_mate())
		printf("%lld %d is thinking\n", time, datas->id);
	else if (num == 5)
		printf("%lli %d died\n", time, datas->id);
	pthread_mutex_unlock(&g_wri);
}

void	*philo_died(t_philo *datas)
{
	if (dead_mate() == 0)
	{
		pthread_mutex_lock(&g_mut);
		print_mess(datas, 5);
		g_dead = 1;
		pthread_mutex_unlock(&g_mut);
	}
	return (NULL);
}

void	release(pthread_mutex_t fork)
{
	pthread_mutex_unlock(&fork);
}

void	release_2(t_philo *datas, int down, int up)
{
	pthread_mutex_unlock(&datas->fork[down]);
	pthread_mutex_unlock(&datas->fork[up]);
}

int	pick_a_fork(t_philo *datas, int f)
{
	if (should_die(datas->feedtime, datas->time_to_die) || dead_mate())
		return (0);
	pthread_mutex_lock(&datas->fork[f]);
	if (should_die(datas->feedtime, datas->time_to_die) || dead_mate())
		return (release(datas->fork[f]), 0);
	return (1);
}

int	philo_eats(t_philo *datas)
{
	if (should_die(datas->feedtime, datas->time_to_die) || dead_mate())
		return (0);
	print_mess(datas, 2);
	if (time_diff(&(datas->feedtime), timenow()) + datas->time_to_eat > datas->time_to_die)
	{
		usleep((datas->time_to_die - (int)time_diff(&(datas->feedtime), timenow())) * 1000);
		return (0);
	}
	datas->nb++;
	usleep(datas->time_to_eat * 1000);
	gettimeofday(&(datas->feedtime), NULL);
	if (should_die(datas->feedtime, datas->time_to_die) || dead_mate())
		return (0);
	return (1);
}

int	philo_sleeps(t_philo *datas)
{
	if (datas->time_to_sleep > datas->time_to_die || dead_mate())
		return (0);
	print_mess(datas, 3);
	usleep(datas->time_to_sleep * 1000);
	if (dead_mate())
		return (0);
	return (1);
}

void    *philo(void *param)
{
	t_philo 		*datas;
	int				up;
	int				down;

	datas = (t_philo*)param;
	down = datas->id - 1;
	//down = down_fork()
	up = datas->id;
	if (datas->id == 0)
		down = datas->nb_philo - 1;
    if (datas->id % 2 == 1)
		usleep(10000);
	while (!should_die(datas->feedtime, datas->time_to_die) && !dead_mate())
	{
		if (!pick_a_fork(datas, up))
			return (philo_died(datas));
		print_mess(datas, 1);//1 pick a fork, 2 is eating, 3 is sleeping, 4 is thinking, died 5
		if (up == down)
            return (release(datas->fork[up]), philo_died(datas));
		if (!pick_a_fork(datas, down))
			return (release(datas->fork[up]), philo_died(datas));
		print_mess(datas, 1);
		if (!philo_eats(datas))
			return (release_2(datas, up, down), philo_died(datas));//shoulddie
		release_2(datas, down, up);
		if (!philo_sleeps(datas) || (datas->nb_feeding != 0 && datas->nb_feeding == datas->nb))
			return ((void *)datas);
        if (!dead_mate())
			print_mess(datas, 4);
	}
	return (philo_died(datas));
}

		/*
		pthread_mutex_lock(&(datas->fork[up]));//LOCK UP
		if (!should_die(feed, datas->time_to_die) && !dead_mate())
		{
			if (!dead_mate())
            	printf("%lld %d has taken a fork\n", time_diff(&datas->start, timenow()), datas->id);
            if (up == down)
            {
				usleep(datas->time_to_die * 1000);
                pthread_mutex_unlock(&(datas->fork[up]));//UNLOCK UP & DIE
            }
			else if (!should_die(feed, datas->time_to_die) && !dead_mate())
            {
				pthread_mutex_lock(&(datas->fork[down]));//LOCK DOWN
				if (dead_mate())
					return (pthread_mutex_unlock(&(datas->fork[down])), pthread_mutex_unlock(&(datas->fork[up])), NULL);//IF dead_mate UNLOCK BOTH
				printf("%lld %d has taken a fork\n", time_diff(&datas->start, timenow()), datas->id);
				if (time_diff(&feed, timenow()) + datas->time_to_eat > datas->time_to_die)
					datas->time_to_eat = datas->time_to_die - (int)time_diff(&feed, timenow());
				if (should_die(feed, datas->time_to_die) || dead_mate())//if should exit
				{
					philo_died(datas);
					pthread_mutex_unlock(&(datas->fork[down]));
					pthread_mutex_unlock(&(datas->fork[up]));//unlock BOTH et RETURN
					return ((void*)datas);
				}	
				printf("%lld %d is eating\n", time_diff(&datas->start, timenow()), datas->id);
                nb++;
		        usleep(datas->time_to_eat * 1000);
                if (should_die(feed, datas->time_to_die) || dead_mate())
				{
					pthread_mutex_unlock(&(datas->fork[down]));
					pthread_mutex_unlock(&(datas->fork[up]));//unlock BOTH et RETURN
					return (philo_died(datas));
		        }
				pthread_mutex_unlock(&(datas->fork[down]));
				pthread_mutex_unlock(&(datas->fork[up]));//IF NOT DEAD UNLOCK BOTHsssssss
				gettimeofday(&feed, NULL);
				printf("%lld %d is sleeping\n", time_diff(&datas->start, timenow()), datas->id);
				if (dead_mate() || datas->time_to_sleep > datas->time_to_die)
					break ;
					//datas->time_to_sleep = datas->time_to_die;
                usleep(datas->time_to_sleep * 1000);
                if (datas->nb_feeding != 0 && datas->nb_feeding == nb)
                    return ((void*)datas);
                if (!dead_mate())
					printf("%lld %d is thinking\n", time_diff(&datas->start, timenow()), datas->id);
            }
			else
				pthread_mutex_unlock(&(datas->fork[up]));//si le philo doit mourir apres la premiere fork unlock avant die
		}
		else
			pthread_mutex_unlock(&(datas->fork[up]));//si le philo doit mourir apres la premiere fork unlock avant die
	*/

t_philo init_thread_datas(int i, char **av, pthread_mutex_t *fork)
{
	t_philo 		new;

    new.nb_philo = atoi(av[1]);
	new.nb = 0;
    gettimeofday(&(new.start), NULL);
	gettimeofday(&(new.feedtime), NULL);
	new.id = i;
    new.time_to_die = atoi(av[2]);
    new.time_to_eat = atoi(av[3]);
    new.time_to_sleep = atoi(av[4]);
	new.nb_feeding = 0;
	if (av[5] != NULL)
		new.nb_feeding = atoi(av[5]);
    new.fork = fork;
    return (new);
}

int main(int ac, char **av)
{
    pthread_t   *pthread;
    int         nb_philo;
    int         i;
    t_philo     *datas;
    int         *r;
    pthread_mutex_t *fork;
	struct timeval	beg;
	struct timeval	end;

	gettimeofday(&beg, NULL);
    i = 0;
    if (ac >= 5)
    {
        nb_philo = atoi(av[1]);
        fork = malloc(sizeof(pthread_mutex_t) * nb_philo);
        if (!fork)
            return (printf("malloc error fork\n"));
        datas = malloc(sizeof(t_philo) * nb_philo);
        if (!datas)
            return (printf("malloc error philo data\n"));
        while (i < nb_philo)
        {
            pthread_mutex_init(&(fork[i]), NULL);
            i++;
        }
        if (!(pthread = (pthread_t *)malloc(sizeof(pthread_t) * nb_philo)))
            return (printf("malloc error pthreads\n"));
        i = 0;
        while (i < nb_philo)//init thread datas + create
        {
            datas[i] = init_thread_datas(i, av, fork);
            pthread_create(&(pthread[i]), NULL, &philo, (void *) &(datas[i]));
            i++;
        }
        i = 0;
        while(i < nb_philo)// boucle des JOIN
        {
            pthread_join(pthread[i], (void **)&r);
			//printf("Le Philo numero %d est revenu avec la valeur %d\n", i, *r);
            i++;
        }
		free(fork);
		i = 0;
		while (i < nb_philo)
		{
			//printf("mutex destroy\n");
			pthread_mutex_destroy(&fork[i]);
			//free(fork + i);
			i++;
		}
    }
	gettimeofday(&end, NULL);
    //printf("TIME= %lld\n", time_diff(&beg, &end));
	return (0);
}
