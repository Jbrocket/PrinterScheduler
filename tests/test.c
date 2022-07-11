#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>

void *thread_fcn(){
	printf("Forking from %u\n", (unsigned int)pthread_self());
	int pid = fork();
	printf("Current process: %d\n", pid);
	if(pid == 0){
		printf("hey\n");
		char *args[] = {"printsim/printersim", "printsim/simple.gcode"};
		execvp("printsim/printersim", args);
	}
	else{
		wait(NULL);
	}
	return NULL;
}

int main(){
	unsigned int NUM_THREADS = 3;
	
	int i;
	pthread_t tid[NUM_THREADS];	
	for(i = 0; i < NUM_THREADS; i++){
		pthread_create(&tid[i], NULL, thread_fcn, NULL);
	}
	for(i = 0; i < NUM_THREADS; i++){
		pthread_join(tid[i], NULL);
	}
	return 0;
}
