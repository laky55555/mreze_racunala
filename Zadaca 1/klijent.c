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


void pomakni_me( int sock );
void odustajem( int sock );


int main( int argc, char **argv )
{
	if( argc != 3 )
		error2( "Upotreba: %s IP-adresa port\n", argv[0] );

	char dekadskiIP[20]; strcpy( dekadskiIP, argv[1] );
	int port;
	sscanf( argv[2], "%d", &port );

	// socket...
	int mojSocket = socket( PF_INET, SOCK_STREAM, 0 );
	if( mojSocket == -1 )
		myperror( "socket" );

	// connect...
	struct sockaddr_in adresaServera;

	adresaServera.sin_family = AF_INET;
	adresaServera.sin_port = htons( port );
	if( inet_aton( dekadskiIP, &adresaServera.sin_addr ) == 0 )
		error2( "%s nije dobra adresa!\n", dekadskiIP );
	memset( adresaServera.sin_zero, '\0', 8 );

	if( connect( mojSocket, (struct sockaddr *) &adresaServera, sizeof( adresaServera ) ) == -1 )
		myperror( "connect" );

    int vrstaPoruke;
    char *pocetnaporuka;

    primiPoruku(mojSocket, &vrstaPoruke, &pocetnaporuka);
    printf("%s", pocetnaporuka);

	// ispisi menu
	int gotovo = 0;
	while( !gotovo )
	{
	    char *poruka;
	    primiPoruku(mojSocket, &vrstaPoruke, &poruka);
	    printf("%s", poruka);

	    if(vrstaPoruke == 5)
            break;

		printf( "\n\nOdaberi opciju...\n"
				"   1. pomicanje\n"
				"   2. odustajanje\n"
				"   \n: " );

		int opcija;
		scanf( "%d", &opcija );

		switch( opcija )
		{
			case 1: pomakni_me( mojSocket ); break;
			case 9: odustajem( mojSocket ); gotovo = 1; break;
			default: printf( "Pogresna opcija...\n" ); break;
		}
	}

	close( mojSocket );

	return 0;
}


void pomakni_me(int sock)
{
    int opcija,j=1;

    while(j)
    {
        printf( "\n\nOdaberi opciju...\n"
                    "   2. pomakni me jedno mjesto dolje\n"
                    "   4. pomakni me jenno mjesto lijevo\n"
                    "   6. pomakni me jedno mjesto desno\n"
                    "   8. pomakni me jedno mjesto gore\n"
                    "   \n: " );
        char *poruka, slovo;
        scanf("%c", &slovo);
        poruka = (char *)malloc(5*sizeof(char));
        sprintf(poruka, "%c", slovo);
        posaljiPoruku(sock, POMAKNI, poruka);

        int vrstaPoruke; char *odgovor;
        if( primiPoruku( sock, &vrstaPoruke, &odgovor ) != OK )
            error1( "Doslo je do pogreske u komunikaciji sa serverom...\n" );

        if( vrstaPoruke != POVRATNA_PORUKA )
            error1( "Doslo je do pogreske u komunikaciji sa serverom (nije poslao ODGOVOR)...\n" );

        if( strcmp( odgovor, "Igrac je pomaknut" ) != 0 )
            printf( "Greska: %s\n", odgovor );
        else
        {
            printf( "Pomak je valjan\n" );
            j=0;
        }

        free( odgovor );
    }
}

void odustajem(int sock)
{
    char *poruka;
    poruka = (char*)malloc(50*sizeof(char));
    sprintf(poruka, "Odustajem\n.");
    posaljiPoruku( sock, ODUSTAJEM, poruka);

	int vrstaPoruke;
	char *odgovor;
	if( primiPoruku( sock, &vrstaPoruke, &odgovor ) != OK )
		error1( "Doslo je do pogreske u komunikaciji sa serverom...\n" );

	if( vrstaPoruke != GOTOVO )
		error1( "Doslo je do pogreske u komunikaciji sa serverom (nije poslao ODGOVOR)...\n" );

	if( strcmp( odgovor, "OK" ) != 0 )
		printf( "Greska: %s\n", odgovor );
	else
	    printf( "OK\n" );
}
