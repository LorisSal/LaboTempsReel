#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* DECLARATION POUR L'UTILISATION DE TIMER*/

timer_t timerid1, timerid2;
int stop = 0;
int h2 = 0;

/*---------------------------------------------------
* GESTIONNAIRE DE SIGNAUX
-------------------------------------------------------*/

void timer_handler1(int sig, siginfo_t *si, void *uc)
{
    printf("Timer 1 fired\n");
	/*---------------------------------------------------
	 * CREATION D'UN TIMER
	 * IDENTIFICATEUR : timeridé
	 * HORLOGE : CLOCK-REALTIME
	 * STRUCTURE SIGEVENT : sev
	 -------------------------------------------------------*/
    struct sigevent sev;
    // sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGUSR2;
    timer_create(CLOCK_REALTIME, &sev, &timerid2);
	
	/*---------------------------------------------------
	 * INITIALISATION D'UN TIMER
	 * IDENTIFICATEUR : timerid2
	 * FLAGS : RELATIVE TIMER
	 * TIMINGS itermspec : its
	 -------------------------------------------------------*/
	 
    struct itimerspec its;
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 500000000;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 500000000;
    timer_settime(timerid2, 0, &its, NULL);

}
void timer_handler2(int sig, siginfo_t *si, void *uc)
{
    printf("Timer 2 fired\n");
    h2++;
    if(h2==10)
    {
		/*---------------------------------------------------
		 * ISUPPRESSION DU TIMER
		 * IDENTIFICATEUR : timerid2
		 -------------------------------------------------------*/
		 
    	timer_delete(timerid2);
		
    	h2 =0;
    }
}

int main(int argc, char *argv[])
{
	/*---------------------------------------------------
	 * CREATION D'UN TIMER
	 * IDENTIFICATEUR : timerid1
	 * HORLOGE : CLOCK-REALTIME
	 * STRUCTURE SIGEVENT : sev1
	 -------------------------------------------------------*/

    struct sigevent sev1;
    //sev1.sigev_notify = SIGEV_SIGNAL;
    sev1.sigev_signo = SIGUSR1;
    timer_create(CLOCK_REALTIME, &sev1, &timerid1);

    /* DECLARATION POUR L'UTILISATION DE SIGNAUX */
    struct sigaction sa1;
    struct sigaction sa2;

	/*---------------------------------------------------
	 * DEFINIR LE COMPORTEMENT DU PROCESSUS COURANT
	 * POUR UN SIGNAL DONNE
	 *
	 * COMPORTEMENT : HANDLERSIGNAL
	 *
	 * SIGNAL  :  SIGUSR, SIGUSR2
	 * FLAGS   :  SA_SIGINFO
	 * MASQUE  :  VIDE
	 -------------------------------------------------------*/
	 
	sa1.sa_flags = SA_SIGINFO;
    sa1.sa_sigaction = timer_handler1;
    sigemptyset(&sa1.sa_mask);

    sa2.sa_flags = SA_SIGINFO;
    sa2.sa_sigaction = timer_handler2;
    sigemptyset(&sa2.sa_mask);


	/*---------------------------------------------------
	 * ARMEMENT SIGNAUX
	 -------------------------------------------------------*/
	 
	sigaction(SIGUSR1, &sa1, NULL);
	sigaction(SIGUSR2, &sa2, NULL);

	 /* DECLARATION POUR L'UTILISATION DE TIMER*/

    struct itimerspec its1;
    time_t act = time(NULL);
    time_t prec = act;

	/*---------------------------------------------------
	 * INITIALISATION D'UN TIMER
	 * IDENTIFICATEUR : timerid1
	 * FLAGS : RELATIVE TIMER
	 * TIMINGS itermspec : its1
	 -------------------------------------------------------*/

	its1.it_value.tv_sec = 2;
    its1.it_value.tv_nsec = 0;
    its1.it_interval.tv_sec = 0;
    its1.it_interval.tv_nsec = 0;
    timer_settime(timerid1, 0, &its1, NULL);
    printf("Timer 1 lunched\n");


    while (!stop) 
	{
    	fflush(STDIN_FILENO);
        if (tcischars(STDIN_FILENO))
			stop = 1;
        act = time(NULL);

        if(act - prec >10)
        {
       		printf("Timer 1 lunched\n");
   		 	timer_settime(timerid1, 0, &its1, NULL);

   		 	prec = act;
        }
    }
	
	/*---------------------------------------------------
	 * ISUPPRESSION DES TIMERS
	 * IDENTIFICATEUR : timerid1, timerid2
	 -------------------------------------------------------*/
	 
    timer_delete(timerid1);
    timer_delete(timerid2);
    printf("Timers stopped\n");

    return 0;
}
