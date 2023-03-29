#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>

#define NbVal 5
#define NAME_MQ_LectToTri "/MQ_LectureToTri"
#define NAME_MQ_TriToAffiche "/MQ_TriToAffichage"

struct data {
    int capt;
    int valeur;
};

int capteur[NbVal][2] = {{6, 3},
						{2, 1},
						{4, 4},
						{4, 3},
						{4, 5}};

void InsertionTriee(struct data *, struct data, int);

int main()
{
    pid_t lecture, affichage, tri;

	mqd_t mqd;
	mqd_t mqd2;

//	mq_unlink(NAME_MQ_LectToTri);
//	mq_unlink(NAME_MQ_TriToAffiche);
//
//	pause();


	struct mq_attr attr;
	attr.mq_flags = 0; // pas de drapeaux
	attr.mq_maxmsg = 50; // nombre maximal de messages dans la file
	attr.mq_msgsize = sizeof(struct data); // taille maximale de chaque message
	attr.mq_curmsgs = 0; // nombre actuel de messages dans la file

	// Creation la file d'attente
	mqd = mq_open(NAME_MQ_LectToTri, O_CREAT, 0666, &attr);
	if (mqd == (mqd_t) -1)
	{
		perror("mq_open Creation");
		exit(EXIT_FAILURE);
	}

	mqd2 = mq_open(NAME_MQ_TriToAffiche, O_CREAT, 0666, &attr);
	if (mqd == (mqd_t) -1)
	{
		perror("mq_open Creation2");
		exit(EXIT_FAILURE);
	}

	mq_close(mqd);
	mq_close(mqd2);

    /*---------------------------------------------------
     * CREATION DU PROCESSUS LECTURE
     *
     * PID : SPAM
     -------------------------------------------------------*/
    if ((lecture = fork()) == -1)
    {
        perror("Erreur fork Lecture\n");
        exit(1);
    }
    if (!lecture)
    {
        /*---------------------------------------------------
         *  CODE PROCESSUS LECTURE
         -------------------------------------------------------*/
		struct data data;
		int i;

		mqd = mq_open(NAME_MQ_LectToTri, O_WRONLY);
        if (mqd == (mqd_t) -1)
        {
            perror("mq_open Processus Lecture");
            exit(EXIT_FAILURE);
        }


		for(i=0;i<NbVal;i++)
        {
			data.capt=capteur[i][0];
			data.valeur=capteur[i][1];

            if (mq_send(mqd, (const char*)&data, sizeof(struct data), 0) == -1)
            {
                perror("mq_send Processus Lecture");
                exit(EXIT_FAILURE);
            }
        }


		mq_close(mqd);


		exit(0);
    }

    /*---------------------------------------------------
     * CREATION DU PROCESSUS TRI
     *
     * PID : tri
     -------------------------------------------------------*/
    if ((tri = fork()) == -1)
    {
        perror("Erreur fork Tri\n");
        exit(1);
    }

    if (!tri)
    {
        /*---------------------------------------------------
         *  CODE PROCESSUS TRI
         -------------------------------------------------------*/
		ssize_t bytes_read;
		struct data data;
		struct data tab[NbVal]={{0}};
		int i, size=0;

		mqd = mq_open(NAME_MQ_LectToTri, O_RDONLY);
        if (mqd == (mqd_t) -1)
        {
            perror("mq_open Processus Tri");
            exit(EXIT_FAILURE);
        }


		mqd2 = mq_open(NAME_MQ_TriToAffiche, O_WRONLY);
        if (mqd2 == (mqd_t) -1)
        {
            perror("mq_open Processus Tri2");
            exit(EXIT_FAILURE);
        }


		for(i=0;i<NbVal;i++)
		{
			bytes_read = mq_receive(mqd, (char*)&data, sizeof(struct data), NULL);
            if (bytes_read == -1)
            {
                perror("mq_receive Processus Tri");
                exit(EXIT_FAILURE);
            }


			//tri


			InsertionTriee(tab, data, size);

			size++;

		}

		for(i=0;i<NbVal;i++)
		{
			if(mq_send(mqd2, (const char*)&tab[i], sizeof(struct data), 0) == -1)
 			{
 				perror("mq_send Processus Tri");
 				exit(EXIT_FAILURE);
 			}
		}

		mq_close(mqd2);
		mq_close(mqd);

		exit(0);
    }

 	/*---------------------------------------------------
 	 * CREATION DU PROCESSUS AFFICHAGE
 	 *
 	 * PID : affichage
 	 -------------------------------------------------------*/
 	if((affichage = fork()) == -1)
 	{
 		perror("Erreur fork processus Affichage \n");
 		exit(1);
 	}
    if(!affichage)
    {

		/*---------------------------------------------------
        *  CODE PROCESSUS Affichage
        ------------------------------------------------------*/
 		ssize_t bytes_read;
		int i;
		struct data data;

		mqd = mq_open(NAME_MQ_TriToAffiche, O_RDONLY);
        if (mqd == (mqd_t) -1)
        {
            perror("mq_open Processus Affichage");
            exit(EXIT_FAILURE);
        }

		for(i=NbVal;i>0;i--)
 		{
 			bytes_read = mq_receive(mqd, (char*)&data, sizeof(struct data), NULL);
 			if (bytes_read == -1)
 			{
 				perror("mq_receive Processus Affichage");
 				exit(EXIT_FAILURE);
 			}
			printf("\nCapteur %d : %d\n", data.capt, data.valeur);


 		}

		mq_close(mqd);
		exit(0);
    }


	/*---------------------------------------------------
	 *  CODE PROCESSUS PERE
	 -------------------------------------------------------*/
    int status;

	waitpid(lecture, &status, 0);
	waitpid(tri, &status, 0);
	waitpid(affichage, &status, 0);

	printf("\n\n\nFIN DES 3 PROCESSUS\n");

	mq_unlink(NAME_MQ_LectToTri);
	mq_unlink(NAME_MQ_TriToAffiche);

	exit(0);

 }


void InsertionTriee(struct data *ptab, struct data x, int size)
{
	printf("Insertion\n");

	int i,j;

	struct data *tmp;
	tmp=ptab;


	for(i=0;i<size && x.capt>ptab->capt;i++,ptab++);

	if(i==size)
	{
		*ptab=x;
	}
	else if(x.capt > ptab->capt)
	{
		for(j=size, ptab=tmp+size-1; j>i ;j--,ptab--)
		{
			*(ptab+1)=*ptab;
		}
		*(ptab+1)=x;
	}
	else
	{
		while(x.valeur<ptab->valeur)
		{
			i++;
			ptab++;
		}
		for(j=size, ptab=tmp+size-1; j>i ;j--,ptab--)
		{
			*(ptab+1)=*ptab;
		}
		*(ptab+1)=x;
	}
}
