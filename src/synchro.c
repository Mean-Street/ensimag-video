#include <pthread.h>
#include <unistd.h>
#include "synchro.h"
#include "ensitheora.h"


/* les variables pour la synchro, ici */
pthread_mutex_t MASTER_MUTEX = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t WINDOW_SIZE = PTHREAD_COND_INITIALIZER;
pthread_cond_t BUFFER_SPACE = PTHREAD_COND_INITIALIZER;
pthread_cond_t TEXTURE = PTHREAD_COND_INITIALIZER;
bool TEX_DONE = false;
int TEXTURE_COUNT = 0;


void tprint(char* str) {
	printf("%d : %s", (int)pthread_self(), str);
}


/* l'implantation des fonctions de synchro ici */
void envoiTailleFenetre(th_ycbcr_buffer buffer) {
	tprint("Entering envoiTailleFenetre.\n");
	pthread_mutex_lock(&MASTER_MUTEX);
	windowsx = buffer[0].width;
	windowsy = buffer[0].height;
	pthread_cond_signal(&WINDOW_SIZE);
	pthread_mutex_unlock(&MASTER_MUTEX);
}

void attendreTailleFenetre() {
	tprint("Entering attendreTailleFenetre.\n");
	pthread_mutex_lock(&MASTER_MUTEX);
	while (windowsx == 0 || windowsy == 0)
		pthread_cond_wait(&WINDOW_SIZE, &MASTER_MUTEX);
	pthread_mutex_unlock(&MASTER_MUTEX);
}

void signalerFenetreEtTexturePrete() {
	tprint("Entering signalerFenetreEtTexturePrete.\n");
	pthread_mutex_lock(&MASTER_MUTEX);
	TEX_DONE = true;
	pthread_cond_signal(&TEXTURE);
	pthread_mutex_unlock(&MASTER_MUTEX);
}

void attendreFenetreTexture() {
	tprint("Entering attendreFenetreTexture.\n");
	pthread_mutex_lock(&MASTER_MUTEX);
	while (!TEX_DONE) {
		pthread_cond_wait(&TEXTURE, &MASTER_MUTEX);
	}
	pthread_mutex_unlock(&MASTER_MUTEX);
}

void debutConsommerTexture() {
	tprint("Entering debutConsommerTexture.\n");
	pthread_mutex_lock(&MASTER_MUTEX);
	while(TEXTURE_COUNT == 0)
		pthread_cond_wait(&BUFFER_SPACE,&MASTER_MUTEX);
}

void finConsommerTexture() {
	tprint("Entering finConsommerTexture.\n");
	TEXTURE_COUNT -= 1;
	if(TEXTURE_COUNT == NBTEX-1)
		pthread_cond_signal(&BUFFER_SPACE);
	pthread_mutex_unlock(&MASTER_MUTEX);
}

void debutDeposerTexture() {
	tprint("Entering debutDeposerTexture.\n");
	pthread_mutex_lock(&MASTER_MUTEX);
	while(TEXTURE_COUNT == NBTEX)
		pthread_cond_wait(&BUFFER_SPACE,&MASTER_MUTEX);
	tprint("Exiting debutDeposerTexture.\n");
}

void finDeposerTexture() {
	tprint("Entering finDeposerTexture.\n");
	TEXTURE_COUNT += 1;
	if(TEXTURE_COUNT == 1)
		pthread_cond_signal(&BUFFER_SPACE);
	pthread_mutex_unlock(&MASTER_MUTEX);
}
