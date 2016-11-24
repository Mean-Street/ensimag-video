#include "synchro.h"
#include "ensitheora.h"


bool fini;

static int WINDOW_WIDTH, WINDOW_HEIGHT;

/* les variables pour la synchro, ici */



/* l'implantation des fonctions de synchro ici */
void envoiTailleFenetre(th_ycbcr_buffer buffer) {
	WINDOW_WIDTH = buffer[0].width;
	WINDOW_HEIGHT = buffer[0].height;
}

void attendreTailleFenetre() {
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
