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

    // ispitujemo uspjesnost povezivanja
    int vrstaPoruke; char *odgovor;
	if( primiPoruku( mojSocket, &vrstaPoruke, &odgovor ) != OK )
		error1( "Doslo je do pogreske u komunikaciji sa serverom...\n" );

	if( vrstaPoruke != SPOJEN )
		error1( "Doslo je do pogreske u konekciji sa serverom (nije poslao SPOJEN)...\n" );

    printf( "%s\n", odgovor );
    int igrac=0;
    if(strcmp(odgovor, "Spojili ste se na server igranja lovice. Vi ste 1. igrac.") ==0)
        igrac=1;
	free( odgovor );


	// ispisi menu
	int gotovo = 0;
	while( !gotovo )
	{
        char *odgovor2;
        if( primiPoruku( mojSocket, &vrstaPoruke, &odgovor2 ) != OK )
            error1( "Doslo je do pogreske u komunikaciji sa serverom (1)...\n" );

        if( vrstaPoruke != NOVA_RUNDA && vrstaPoruke != GOTOVO)
            error1( "Doslo je do pogreske u komunikaciji sa serverom (nije poslao NOVA RUNDA) (2)...\n" );

        printf( "%s\n", odgovor2 );

        free( odgovor2 );

	    if(vrstaPoruke == GOTOVO)
            break;
        if (igrac == 0)
            printf( "Odaberi opciju...\n"
                    "   1. pomicanje\n"
                    "   3. odustajanje\n"
                    "   \n: " );
        if (igrac == 1)
            printf( "Odaberi opciju...\n"
                    "   1. pomicanje\n"
                    "   \n: " );

		int opcija;
		scanf( "%d", &opcija );
        if (opcija != 1 && opcija !=3)
        {
            printf("Pogresna opcija, odaberi ponovo.\n");
            scanf("%d", &opcija);
        }
		switch( opcija )
		{
			case 1: pomakni_me( mojSocket ); break;
			case 3: odustajem( mojSocket );  break;
		}
	}

	close( mojSocket );

	return 0;
}

void pomakni_me(int sock)
{
    int kraj=0;
    while(!kraj)
    {
        printf("Upisi u kojem smjeru se zelis kretati: gore, dolje, lijevo, desno.\n");
        char smjer[10];
        scanf("%s", smjer);
        while(strcmp(smjer, "lijevo") != 0 && strcmp(smjer, "desno") != 0 && strcmp(smjer, "gore") != 0 && strcmp(smjer, "dolje") != 0)
        {
            printf("Unos je kriv, moze se kretati samo: lijevo, desno, gore, dolje\n");
            scanf("%s", smjer);
        }
        posaljiPoruku(sock, POMAKNI, smjer);


        int vrstaPoruke; char *odgovor;
        if( primiPoruku( sock, &vrstaPoruke, &odgovor ) != OK )
            error1( "Doslo je do pogreske u komunikaciji sa serverom (3)...\n" );
        if( vrstaPoruke != POVRATNA_PORUKA )
            error1( "Doslo je do pogreske u komunikaciji sa serverom (nije poslao ODGOVOR) (4)...\n" );
        printf( "%s\n", odgovor );
        if (strcmp(odgovor, "Poruka je obradena") == 0)
            kraj = 1;
        free( odgovor );
    }
}


void odustajem(int sock)
{
    char kraj[15];
    sprintf(kraj, "Odustajem");
    posaljiPoruku(sock, ODUSTAJEM, kraj);

    int vrstaPoruke; char *odgovor;
    if( primiPoruku( sock, &vrstaPoruke, &odgovor ) != OK )
        error1( "Doslo je do pogreske u komunikaciji sa serverom (5)...\n" );

    if( vrstaPoruke != ODUSTAJEM )
        error1( "Doslo je do pogreske u komunikaciji sa serverom (nije poslao POVRATNA PORUKA) (6)...\n" );
    printf( "%s\n", odgovor );
    free( odgovor );

}











