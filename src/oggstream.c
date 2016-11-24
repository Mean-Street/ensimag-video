#include <stdio.h>
#include <assert.h>
#include <ogg/ogg.h>
#include "stream_common.h"
#include "ensitheora.h"
#include "ensivorbis.h"
#include "synchro.h"


ogg_sync_state oggtheorastate, oggvorbisstate; /* sync and verify incoming physical bitstream */
ogg_page theorapage, vorbispage; /* one Ogg bitstream page. Vorbis packets are inside */


/* parse the file and read pages */
void *theoraStreamReader(void *arg) {
	char *filename = (char *) arg;

	FILE *vf = fopen(filename, "r");
	assert(vf);

	// Initialisation de l'automate OGG
	int res = ogg_sync_init( &oggtheorastate );
	assert(res == 0);

	int respac = 0;
	struct streamstate *s;

	while(! fini) {
		//printf("theora loop\n");// vérifier si le fichier ne serait pas fini
		if ( feof( vf ) ) {
			fini = true;
			return 0;
		}

		if (respac == 0) {
			pageReader(vf, &oggtheorastate, &theorapage);
			s = getStreamState(&oggtheorastate, &theorapage, TYPE_THEORA);

			// ignore vorbis stream
			if (s->strtype == TYPE_VORBIS)
				continue;

			// add the page and try to get a complete packet
			respac = addPageGetPacket(&theorapage, s);
		} else {
			// just try to get a complete packet (when is it necessary ??)
			respac = getPacket(s);
		}	    
		
		// if respac = 1, we completed a packet and
		// we can now decode what is inside ...
		switch( respac ) {
			case -1:
				// data is fucked up man
				s->nbpacketoutsync++;
				printf("out of sync: gap in data\n");
				break;
			case 0:
				// end of a page, but no complete packet
				// more pages (data) are needed to build a full packet 
				continue;
				break;
			case 1:
				// nice, we've got a packet
				s->nbpacket++;
				break;
		}

		// ... we first check if it's a header
		if ( decodeAllHeaders(respac, s, TYPE_THEORA) )
			continue;

		// if it's not, a thread with draw2SDL has been launched
		// it's data, let's extract it
		if (s->strtype == TYPE_THEORA && s->headersRead) {
			theora2SDL(s);
		}

	}
	fclose(vf);
	return 0;
}

void *vorbisStreamReader(void *arg) {
	char *filename = (char *) arg;

	FILE *vf = fopen(filename, "r");
	assert(vf);

	// Initialisation de l'automate OGG
	int res = ogg_sync_init( &oggvorbisstate );
	assert(res == 0);

	int respac = 0;
	struct streamstate *s;

	while(! fini) {
		// printf ("vorbis loop \n");
		// vérifier si le fichier ne serait pas fini
		if ( feof( vf ) ) {
			fini = true;
			printf("FIN de la lecture de VORBIS !");
			break;
		}

		if (respac == 0) {
			pageReader(vf, & oggvorbisstate, & vorbispage);
			s = getStreamState(& oggvorbisstate,
					& vorbispage,
					TYPE_VORBIS);

			// ignorer le stream theora
			if (s->strtype == TYPE_THEORA)
				continue;

			// ajouter la page dans le décodeur et tenter d'extraire un
			// packet
			respac = addPageGetPacket(& vorbispage, s);
		} else {
			respac = getPacket(s);
		}

		switch( respac ) {
			case -1:
				s->nbpacketoutsync++;
				printf("out of sync: gap in data\n");
				break;
			case 0:
				// more pages (data) are needed to build a full packet 
				continue;
				break;
			case 1:
				s->nbpacket++;
				break;
		}

		// Vorbis calls it with TYPE_THEORA ?? Dafuq is that
		if ( decodeAllHeaders(respac, s, TYPE_THEORA) )
			continue;

		// boucle principale de lecture vorbis
		if (s->strtype == TYPE_VORBIS && s->headersRead) {
			vorbis2SDL(s);
		}
	}

	fclose(vf);
	return 0;
}
