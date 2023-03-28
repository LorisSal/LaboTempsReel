#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>

#define NB 1024
#define SIGNAL SIGUSR1

void handlerSIGNAL(int sig);

int compteur;

int main()
{
	/* DECLARATION POUR L'UTILISATION DE SIGNAUX */

	struct sigaction sig_act;
	int spam, i=0, ret;
	
	
	pid_t pid;
	pid = getpid();

	/*---------------------------------------------------
	 * DEFINIR LE COMPORTEMENT DU PROCESSUS COURANT
	 * POUR UN SIGNAL DONNE
	 *
	 * COMPORTEMENT : HANDLERSIGNAL
	 *
	 * SIGNAL  :  SIGNAL
	 * FLAGS   :  SA_SIGINFO
	 -------------------------------------------------------*/

	sig_act.sa_handler = handlerSIGNAL;
	sig_act.sa_flags = SA_SIGINFO;

	/*---------------------------------------------------
	 * ARMEMENT SIGNAL
	 -------------------------------------------------------*/
	 
	ret = sigaction(SIGNAL, &sig_act, 0);
	if(ret == -1)
	{
		perror("\nMain : ERREUR sigaction\n");
	}
	
	/*---------------------------------------------------
	 * CREATION DU PROCESSUS SPAM
	 *
	 * PID : SPAM
	 -------------------------------------------------------*/
	 
    if((spam = fork()) == -1)
	{
		perror("Erreur fork 1 \n");
		exit(1);
	}
    if(!spam)
    {
		/*---------------------------------------------------
		 *  CODE PROCESSUS SPAM
		 -------------------------------------------------------*/
		 
		printf("Lancement du processus spam\n");
		while(i<NB)
		{
			/*---------------------------------------------------
			 * ENVOIE D'UNE RAFALE DE SIGNAUX AU PROCESSUS PERE
			 -------------------------------------------------------*/
			 
			kill(pid, SIGNAL);
			i++;
		}
    }
    else
    {
		/*---------------------------------------------------
		 *  CODE PROCESSUS PERE
		 -------------------------------------------------------*/
		 
    	for(i=0;i<NB;i++)
    		pause();
    }

    exit(0);

}

/*---------------------------------------------------
* GESTIONNAIRE DE SIGNAL
-------------------------------------------------------*/

void handlerSIGNAL(int sig)
{
	compteur++;
	printf("\nCompteur = %d", compteur);
}
