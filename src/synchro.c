#include <pthread.h>
#include <unistd.h>
#include "synchro.h"
#include "ensitheora.h"


bool fini;

/* les variables pour la synchro, ici */
pthread_mutex_t window_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t window_size = PTHREAD_COND_INITIALIZER;


/* l'implantation des fonctions de synchro ici */
void envoiTailleFenetre(th_ycbcr_buffer buffer) {
	pthread_mutex_lock(&window_mutex);

	printf("ENVOI TAILLE FENETRE.\n");
	windowsx = buffer[0].width;
	windowsy = buffer[0].height;
	pthread_cond_signal(&window_size);

	pthread_mutex_unlock(&window_mutex);
}

void attendreTailleFenetre() {
	pthread_mutex_lock(&window_mutex);

	while (windowsx == 0 || windowsy == 0)
		pthread_cond_wait(&window_size,&window_mutex);
	printf("ATTENDRE TAILLE FENETRE.\n");

	pthread_mutex_unlock(&window_mutex);
}

void signalerFenetreEtTexturePrete() {
}

void attendreFenetreTexture() {
}

void debutConsommerTexture() {
}

void finConsommerTexture() {
}

void debutDeposerTexture() {
}

void finDeposerTexture() {
}
