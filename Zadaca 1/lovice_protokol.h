#ifndef __SKLADISTEPROTOCOL_H_
#define __SKLADISTEPROTOCOL_H_

#define SPOJEN           1
#define POMAKNI          2
#define ODUSTAJEM        3
#define NOVA_RUNDA       4
#define POVRATNA_PORUKA  5
#define GOTOVO           6

// ovo ispod koriste i klijent i server, pa moze biti tu...
#define OK      1
#define NIJEOK  0

int primiPoruku( int sock, int *vrstaPoruke, char **poruka );
int posaljiPoruku( int sock, int vrstaPoruke, char *poruka );

#define error1( s ) { printf( s ); exit( 0 ); }
#define error2( s1, s2 ) { printf( s1, s2 ); exit( 0 ); }
#define myperror( s ) { perror( s ); exit( 0 ); }

#endif
