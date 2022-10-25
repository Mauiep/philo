/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: admaupie <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/17 18:02:21 by admaupie          #+#    #+#             */
/*   Updated: 2022/10/25 21:59:22 by admaupie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "philo.h"
#include <sys/time.h>

int	take_a_fork()
{
	return (0);
}

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

void    *philo(void *param)
{
    t_philo 		*datas;
    int             up;
    int             down;
    struct timeval  feed;
    int             nb;

    nb = 0;
    datas = (t_philo*)param;
    down = datas->id - 1;
    up = datas->id;
    if (datas->id == 0)
        down = datas->nb_philo - 1;
    feed = datas->start;
    if (datas->id % 2 == 1)
		usleep(100);
	while (!should_die(feed, datas->time_to_die))
	{
//		printf("Philo %d : time to die = %lld et now = %lld\n", datas->id, datas->time_to_die, time_diff(&feed, timenow()));
		pthread_mutex_lock(&(datas->fork[up]));
        if (!should_die(feed, datas->time_to_die))
		{
            printf("%lld %d has taken a fork\n", time_diff(&datas->start, timenow()), datas->id);
            if (up == down)
            {
                pthread_mutex_unlock(&(datas->fork[up]));
                usleep(datas->time_to_die * 1000);
            }
			else if (!should_die(feed, datas->time_to_die))
            {
				pthread_mutex_lock(&(datas->fork[down]));
                printf("%lld %d has taken a fork\n", time_diff(&datas->start, timenow()), datas->id);
                if (datas->time_to_eat > datas->time_to_die)
					datas->time_to_eat = datas->time_to_die;
				if (should_die(feed, datas->time_to_die))
				{
                	pthread_mutex_unlock(&(datas->fork[down]));
		        	pthread_mutex_unlock(&(datas->fork[up]));
					printf("%lli %d died\n", time_diff(&datas->start, timenow()), datas->id);
					return ((void*)datas);
				}
				printf("%lld %d is eating\n", time_diff(&datas->start, timenow()), datas->id);
                nb++;
		        usleep(datas->time_to_eat * 1000);
                gettimeofday(&feed, NULL);
		        pthread_mutex_unlock(&(datas->fork[down]));
                pthread_mutex_unlock(&(datas->fork[up]));
				printf("%lld %d is sleeping\n", time_diff(&datas->start, timenow()), datas->id);
				if (datas->time_to_sleep > datas->time_to_die)
					break ;
					//datas->time_to_sleep = datas->time_to_die;
                usleep(datas->time_to_sleep * 1000);
                if (datas->nb_feeding != 0 && datas->nb_feeding == nb)
                    return ((void*)datas);
                printf("%lld %d is thinking\n", time_diff(&datas->start, timenow()), datas->id);
            }
		}
		else
			pthread_mutex_unlock(&(datas->fork[down]));
	}
    //while (time_diff(&feed, timenow()) < datas->time_to_die)
    //    nb++;
	printf("%lli %d died\n", time_diff(&datas->start, timenow()), datas->id);
	return ((void*)datas);
}

t_philo init_thread_datas(int i, char **av, pthread_mutex_t *fork)
{
	t_philo 		new;

    new.nb_philo = atoi(av[1]);
    gettimeofday(&(new.start), NULL);
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
    }
	gettimeofday(&end, NULL);
    //printf("TIME= %lld\n", time_diff(&beg, &end));
	return (0);
}
