#include <pthread.h>
#include <unistd.h>
#include "synchro.h"
#include "ensitheora.h"


bool fini;

/* les variables pour la synchro, ici */
pthread_mutex_t window_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t window_size = PTHREAD_COND_INITIALIZER;

pthread_cond_t texture = PTHREAD_COND_INITIALIZER;


/* l'implantation des fonctions de synchro ici */
void envoiTailleFenetre(th_ycbcr_buffer buffer) {
	pthread_mutex_lock(&window_mutex);

	/* printf("ENVOI TAILLE FENETRE.\n"); */
	windowsx = buffer[0].width;
	windowsy = buffer[0].height;
	pthread_cond_signal(&window_size);

	pthread_mutex_unlock(&window_mutex);
}

void attendreTailleFenetre() {
	pthread_mutex_lock(&window_mutex);

	while (windowsx == 0 || windowsy == 0)
		pthread_cond_wait(&window_size, &window_mutex);
	/* printf("ATTENDRE TAILLE FENETRE.\n"); */

	pthread_mutex_unlock(&window_mutex);
}

void signalerFenetreEtTexturePrete() {
	pthread_mutex_lock(&window_mutex);
	tex_done = true;
	pthread_cond_signal(&texture);
	pthread_mutex_unlock(&window_mutex);
}

void attendreFenetreTexture() {
	pthread_mutex_lock(&window_mutex);
	while (! tex_done) {
		pthread_cond_wait(&texture, &window_mutex);
	}
	tex_done = false;
	pthread_mutex_unlock(&window_mutex);
}

void debutConsommerTexture() {
	pthread_mutex_lock(&window_mutex);
	pthread_mutex_unlock(&window_mutex);
}

void finConsommerTexture() {
	pthread_mutex_lock(&window_mutex);
	pthread_mutex_unlock(&window_mutex);
}

void debutDeposerTexture() {
	pthread_mutex_lock(&window_mutex);
	pthread_mutex_unlock(&window_mutex);
}

void finDeposerTexture() {
	pthread_mutex_lock(&window_mutex);
	pthread_mutex_unlock(&window_mutex);
}
