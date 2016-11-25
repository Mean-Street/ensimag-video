#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <pthread.h>

#include "stream_common.h"
#include "oggstream.h"

pthread_mutex_t hash_mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]) {
	int res;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s FILE\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	assert(argc == 2);


	// Initialisation de la SDL
	res = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_EVENTS);
	atexit(SDL_Quit);
	assert(res == 0);

	// start the two stream readers
	pthread_create(&video_thread, NULL, theoraStreamReader, (void*) argv[1]);
	/* pthread_create(&audio_thread, NULL, vorbisStreamReader, (void*) argv[1]); */

	// wait audio thread
	/*pthread_join(audio_thread,NULL);*/

	// 1 seconde de garde pour le son,
	sleep(1);

	// tuer les deux threads videos si ils sont bloqués
	pthread_cancel(video_thread);
	pthread_cancel(gui_thread);

	// attendre les 2 threads videos
	pthread_join(video_thread,NULL);	
	pthread_join(gui_thread,NULL);	

	exit(EXIT_SUCCESS);    
}
