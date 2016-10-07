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

int pomak_igraca(int *x, int *y, char *kud, char **matrica, int a)
{
    if (strcmp(kud, "2"))
    {
        if(matrica[*x][*y-1] == '.')
        {
            matrica[*x][*y] = '.';
            if (a==1)
                matrica[*x][*y-1] = 'X';
            else
                matrica[*x][*y-1] = 'Y';
            (*y)--;
            return 1;
        }
        else if(matrica[*x][*y-1] == '#')
            return 0;
        else
            return 2;
    }
    if (strcmp(kud, "4"))
    {
        if(matrica[*x-1][*y] == '.')
        {
            matrica[*x][*y] = '.';
            if (a==1)
                matrica[*x-1][*y] = 'X';
            else
                matrica[*x-1][*y] = 'Y';
            (*x)--;
            return 1;
        }
        else if(matrica[*x-1][*y] == '#')
            return 0;
        else
            return 2;
    }
    if (strcmp(kud, "6"))
    {
        if(matrica[*x][*y+1] == '.')
        {
            matrica[*x][*y] = '.';
            if (a==1)
                matrica[*x][*y+1] = 'X';
            else
                matrica[*x][*y+1] = 'Y';
            (*y)++;
            return 1;
        }
        else if(matrica[*x][*y+1] == '#')
            return 0;
        else
            return 2;
    }
    if (strcmp(kud, "8"))
    {
        if(matrica[*x+1][*y] == '.')
        {
            matrica[*x][*y] = '.';
            if (a==1)
                matrica[*x+1][*y] = 'X';
            else
                matrica[*x+1][*y] = 'Y';
            (*y)--;
            return 1;
        }
        else if(matrica[*x+1][*y] == '#')
            return 0;
        else
            return 2;
    }
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
            }
            if(matrica[i][j] == 'Y')
            {
                Yx = i;
                Yy = j;
            }
        }
    if (Xx == Yx && Xy == Yy)
        perror("Labirint je lose zadan");
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
	while( i!=2 ) // vjecno cekamo nove klijente...
	{
		// accept...
		struct sockaddr_in klijentAdresa;
		int lenAddr = sizeof( klijentAdresa );
		int commSocket = accept( listenerSocket, (struct sockaddr *) &klijentAdresa, &lenAddr );
//g++ messageProtocol.h messageProtocol.c messageServer.c messageClient.c
		if( commSocket == -1 )
			perror( "accept" );

		char *dekadskiIP = inet_ntoa( klijentAdresa.sin_addr );
		printf( "Prihvatio konekciju od %s ", dekadskiIP );
		char dobrodosli[100];
		sprintf(dobrodosli, "Spojili ste se na server igranja lovice. Vi ste %d igrac.", i+1);
		posaljiPoruku(commSocket, SPOJEN, dobrodosli);
        uticnica[i] = commSocket;
        i++;
	}

	int kraj = 0;
	while (!kraj)
    {
        char *pocetna;
        pocetna = (char *)malloc(50*sizeof(char));
        sprintf(pocetna, "Pocinje nova runda\n");
        posaljiPoruku(uticnica[0], POVRATNA_PORUKA, pocetna);
        posaljiPoruku(uticnica[1], POVRATNA_PORUKA, pocetna);


        char *losa, *dobra;
        losa = (char *)malloc(50*sizeof(char));
        dobra = (char *)malloc(50*sizeof(char));
        sprintf(losa, "U smjeru tog kretanja je zid");
        sprintf(dobra, "Igrac je pomaknut");
        while(1)                                                //micanje prvog igraca, ako se makne na drugog igra treba zavrsiti ->kraj = 5
        {
            int broj_poruke, test;
            char *poruka;
            primiPoruku(uticnica[0], &broj_poruke, &poruka);
            if (broj_poruke != 1)
                error1("Kriva poruka");
            test = pomak_igraca(&Xx, &Xy, poruka, matrica, 1);
            if (test == 1)
            {
                posaljiPoruku(uticnica[0], 3, dobra);
                break;
            }
            else if (test == 0)
                posaljiPoruku(uticnica[0], 3, losa);
            else
            {
                kraj = 1;
                break;
            }
        }
        if (kraj == 1) break;
        while(1)                                                //micanje drugog igraca, ako se makne na prvog igra treba zavrsiti ->kraj = 5
        {
            int broj_poruke, test;
            char *poruka;
            primiPoruku(uticnica[1], &broj_poruke, &poruka);
            if (broj_poruke != 1 || broj_poruke != 2)
                error1("Kriva poruka");
            if (broj_poruke == 2)
            {
                kraj = 5;
                break;
            }
            test = pomak_igraca(&Yx, &Yy, poruka, matrica, 1);
            if (test == 1)
            {
                posaljiPoruku(uticnica[0], 3, dobra);
                break;
            }
            else if (test == 0)
                posaljiPoruku(uticnica[0], 3, losa);
            else
            {
                kraj = 1;
                break;
            }
        }
    }


    char *poruka;
    poruka = (char *)malloc(50*sizeof(char));
    if (kraj == 1)
        sprintf(poruka, "Pobjedio je 2 igrac.\n");
    else
        sprintf(poruka, "Pobjedio je 1 igrac.\n");

    posaljiPoruku(uticnica[0], GOTOVO, poruka);
    posaljiPoruku(uticnica[1], GOTOVO, poruka);

    close(uticnica[0]);
    close(uticnica[1]);
    close(listenerSocket);


    for (i=0; i<100; i++)
        free(matrica[i]);
    free(matrica);
    return 0;
}
