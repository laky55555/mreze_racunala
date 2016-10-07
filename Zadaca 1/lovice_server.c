#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "lovice_protokol.h"


int pomak_igraca(char **matrica, int *x, int *y, char *kud)
{
    if (strcmp(kud, "lijevo") == 0)
    {
        if(matrica[*x][*y-1] == '#')
            return 0;
        else
        {
            (*y)--;
            return 1;
        }
    }
    else if (strcmp(kud, "desno") == 0)
    {
        if(matrica[*x][*y+1] == '#')
            return 0;
        else
        {
            (*y)++;
            return 1;
        }
    }
    else if (strcmp(kud, "gore") == 0)
    {
        if(matrica[*x-1][*y] == '#')
            return 0;
        else
        {
            (*x)--;
            return 1;
        }
    }
    else if (strcmp(kud, "dolje") == 0)
    {
        if(matrica[*x+1][*y] == '#')
            return 0;
        else
        {
            (*x)++;
            return 1;
        }
    }
    else
        return 0;
}


char **ucitavanje(char **matrica, int *retci, int *stupci)
{
    FILE *in;
    char linija[200], ime_datoteke[20];
    int i,k;
    sprintf(ime_datoteke, "labirint.txt");
    if ((in = fopen(ime_datoteke, "rt")) == NULL)
    {
        printf("Greska prilikom otvaranja datoteke \"%s\".\n", ime_datoteke);
        return 0;
    }
    i=0;
    while(1)
    {
        fgets(linija, 200,in);
        if (strcmp(linija, "kraj") == 0)
            break;
        (*stupci) = strlen(linija);
        for (k=0; k<(*stupci); k++)
        {
            matrica[i][k] = linija[k];
           // printf("%c", matrica[i][k]);
        }
        i++;
    }
    (*stupci)--;
    (*retci) = i;
   // printf("\n\n%d   %d", redci,stupci);

    fclose(in);
    return matrica;
}


int main(int argc, char** argv)
{
    int retci, stupci, i ,j;
    char **matrica;
    matrica =(char **)malloc(100*sizeof(char*));
    for(i=0; i<100; i++)
        matrica[i] = (char *)malloc(100*sizeof(char));
    matrica = ucitavanje(matrica, &retci, &stupci);
    /*for(i=0;i<retci; i++)
    {
        for(j=0; j<stupci; j++)
            printf("%c", matrica[i][j]);
        printf("\n");
    }*/

    //koordinate X, Y
    int Xx, Xy, Yx, Yy;
    for(i=0;i<retci; i++)
        for(j=0; j<stupci; j++)
        {
            if(matrica[i][j] == 'X')
            {
                Xx = i;
                Xy = j;
                matrica[i][j] == '.';
            }
            if(matrica[i][j] == 'Y')
            {
                Yx = i;
                Yy = j;
                matrica[i][j] == '.';
            }
        }

   // printf("X(%d, %d)   Y(%d, %d)", Xx, Xy, Yx, Yy);


    if( argc != 2 )
		error2( "Upotreba: %s port\n", argv[0] );

	int port;
	sscanf( argv[1], "%d", &port );

	// socket...
	int listenerSocket = socket( PF_INET, SOCK_STREAM, 0 );
	if( listenerSocket == -1 )
	    perror( "socket" );

	// bind...
	struct sockaddr_in mojaAdresa;

	mojaAdresa.sin_family      = AF_INET;
	mojaAdresa.sin_port        = htons( port );
	mojaAdresa.sin_addr.s_addr = INADDR_ANY;
	memset( mojaAdresa.sin_zero, '\0', 8 );

	if( bind(listenerSocket, (struct sockaddr *) &mojaAdresa, sizeof( mojaAdresa ) ) == -1 )
		perror( "bind" );

	// listen...
	if( listen( listenerSocket, 2 ) == -1 )
		perror( "listen" );

    i = 0;
    int uticnica[2];
	while( i!=2 ) // cekamo 2 klijenta
	{
		// accept...
		struct sockaddr_in klijentAdresa;
		int lenAddr = sizeof( klijentAdresa );
		int commSocket = accept( listenerSocket, (struct sockaddr *) &klijentAdresa, &lenAddr );
//g++ messageProtocol.h messageProtocol.c messageServer.c messageClient.c
		if( commSocket == -1 )
			perror( "accept" );

		char *dekadskiIP = inet_ntoa( klijentAdresa.sin_addr );
		printf( "Prihvatio konekciju od %s \n", dekadskiIP );
		char dobrodosli[100];
		sprintf(dobrodosli, "Spojili ste se na server igranja lovice. Vi ste %d. igrac.", i+1);
		posaljiPoruku(commSocket, SPOJEN, dobrodosli);
        uticnica[i] = commSocket;
        i++;
        free(dekadskiIP);
	}

    int kraj = 0, vrstaPoruke; i=1;
    char uspjesan[40], neuspjesan[40];
    sprintf(uspjesan, "Poruka je obradena");
    sprintf(neuspjesan, "Pomak je neuspjesan, pazi zid");
    while (!kraj)                // kraj = 1..pobjedio je prvi   kraj = 2....pobjedio je drugi
    {
        char pocetna[30];
        sprintf(pocetna, "Pocinje %d. runda\n", i);

        posaljiPoruku(uticnica[0], NOVA_RUNDA, pocetna);
        while(1)
        {
            char *odgovor;
            if( primiPoruku(uticnica[0], &vrstaPoruke, &odgovor ) != OK )
                error1( "Doslo je do pogreske u komunikaciji sa klijentom...\n" );
            if( vrstaPoruke != POMAKNI )
                error1( "Doslo je do pogreske u komunikaciji sa klijentom (nije poslao POMAK)...\n" );

            if ( pomak_igraca(matrica, &Xx, &Xy, odgovor) == 0)
                posaljiPoruku(uticnica[0], POVRATNA_PORUKA, neuspjesan);
            else
            {
                posaljiPoruku(uticnica[0], POVRATNA_PORUKA, uspjesan);
                break;
            }
            free( odgovor );
        }

        if (Xx == Yx && Xy == Yy)
        {
            kraj = 2;
            break;
        }

        posaljiPoruku(uticnica[1], NOVA_RUNDA, pocetna);
        while(1)
        {
            char *odgovor;
            if( primiPoruku( uticnica[1], &vrstaPoruke, &odgovor ) != OK )
                error1( "Doslo je do pogreske u komunikaciji sa klijentom...\n" );
            if( vrstaPoruke == POMAKNI)
            {
                if ( pomak_igraca(matrica, &Yx, &Yy, odgovor) == 0)
                    posaljiPoruku(uticnica[1], POVRATNA_PORUKA, neuspjesan);
                else
                {
                    posaljiPoruku(uticnica[1], POVRATNA_PORUKA, uspjesan);
                    if (Xx == Yx && Xy == Yy)
                        kraj = 2;
                    break;
                }
                free( odgovor );
            }
            else if( vrstaPoruke == ODUSTAJEM)
            {
                kraj = 1;
                posaljiPoruku(uticnica[1], ODUSTAJEM, uspjesan);
                free(odgovor);
                break;
            }
            else
                error1( "Doslo je do pogreske u komunikaciji sa klijentom (nije poslao POMAK ili ODUSTAJEM)...\n" );
        }
        i++;
    }

    char prvi[30], drugi[30];
    sprintf(prvi, "Pobjedio je prvi igrac");
    sprintf(drugi, "Pobjedio je drugi igrac");
    if (kraj == 1)
    {
        posaljiPoruku(uticnica[0], GOTOVO, prvi);
        posaljiPoruku(uticnica[1], GOTOVO, prvi);
    }
    else
    {
        posaljiPoruku(uticnica[0], GOTOVO, drugi);
        posaljiPoruku(uticnica[1], GOTOVO, drugi);
    }

    close(uticnica[0]);
    close(uticnica[1]);
    close(listenerSocket);


    for (i=0; i<100; i++)
        free(matrica[i]);
    free(matrica);

    return 0;
}
