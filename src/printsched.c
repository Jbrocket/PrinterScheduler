#include "../inc/dll.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

#include <sys/wait.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int num_jobs = 0;
int is_quit = 0;
dll_t *job_queue;
int algorithm = 0;


void *printer_handler();
void print_jobs();
int queue_wait(int job_id);
int drain_queue();
node_t *smallest_job();
node_t *smallest_job_with_wait_prio();

int main(int argc, char *argv[]){
	int job_id = 0;
	job_queue = (dll_t *)malloc(sizeof(dll_t));
	job_queue->head = NULL;
	job_queue->tail = NULL;
	job_queue->length = 0;



	if(argc != 2){
		fprintf(stdout, "Usage: printsched [number of printers]\n");
		exit(-1);
	}
	

	long unsigned int NUM_THREADS = strtoul(argv[1], NULL, 10);
	if(NUM_THREADS == 0){
		fprintf(stderr, "Error: Invalid number of threads (Must be a number > 0)\n");
		exit(-1);
	}
	else if(NUM_THREADS == ULONG_MAX){
		fprintf(stderr, "Error: Valid too large\n");
		exit(-1);
	}
	unsigned int i;
	
	
	pthread_t tid[NUM_THREADS];

	// Set arguments
	for(i = 0; i < NUM_THREADS; i++){
		// Do stuff with arguments here
	}
	
	// Create printers
	for(i = 0; i < NUM_THREADS; i++){
		if(pthread_create(&tid[i], NULL, printer_handler, NULL) != 0){
			fprintf(stderr, "Error: pthread_create failed.\n %s\n", strerror(errno));
			exit(-1);
		}
	}	

	// Handle I/O
	unsigned int buffer_length = 256;
	char buffer[buffer_length];
	char *splits[buffer_length];
    fprintf(stdout, "type \"help\" to begin\n");
	while(fgets(buffer, buffer_length, stdin) != NULL && !is_quit){
		int num_args = 1;
		char *temp = strtok(buffer, " \t\n");
		splits[0] = temp;
		while(1){
			temp = strtok(NULL, " \t\n");
			if(temp == NULL) break;
			splits[num_args] = temp;
			num_args++;
		
		}

		if(!strcmp(splits[0], "submit")){
			if(num_args != 2){
				fprintf(stdout, "Usage: submit <file name>\n");
				
			}
			else{
				struct stat stat_buff;
				if(stat(splits[1], &stat_buff) < 0){
					fprintf(stderr, "Error: stat failed.\n%s\n", strerror(errno));
				}	
				else{
					node_t *temp_node = create_node((int)stat_buff.st_size, job_id, time(NULL), splits[1]);
					job_id++;
					pthread_mutex_lock(&lock);
					push_back(job_queue, temp_node);
					num_jobs++;
					pthread_cond_broadcast(&cond);
					pthread_mutex_unlock(&lock);
				}				
			}
		}
		else if(!strcmp(splits[0], "list")){
			if(num_args != 1){
				fprintf(stdout, "Usage: list\n");
			}
			else{
				print_jobs();
			}
		}
		else if(!strcmp(splits[0], "wait")){
			if(num_args != 2){
				fprintf(stdout, "Usage: wait <job id>\n");
			}
			else{
				int job_id = strtol(splits[1], NULL, 10);
				queue_wait(job_id);




			}
		}
		else if(!strcmp(splits[0], "drain")){
			if(num_args != 1){
				fprintf(stdout, "Usage: drain\n");
			}
			else{
				drain_queue();
			}
		}
		else if(!strcmp(splits[0], "hurry")){
			if(num_args != 2){
				fprintf(stdout, "Usage: hurry <job id>\n");
			}
			else{
				fprintf(stdout, "%s\n", splits[1]);
			}
		}
		else if(!strcmp(splits[0], "remove")){
			if(num_args != 2){
				fprintf(stdout, "Usage: remove <job id>\n");
			}
			else{
				fprintf(stdout, "%s\n", splits[1]);
			}
		}
		else if(!strcmp(splits[0], "algo")){
			if(num_args != 2){
				fprintf(stdout, "Usage: algo <fifo|sjf|balanced>\n");
			}
			else{
				if(!strcmp(splits[1], "fifo")){
					algorithm = 0;
				}
				else if(!strcmp(splits[1], "sjf")){
					algorithm = 1;
				}
				else if(!strcmp(splits[1], "balanced")){
					algorithm = 2;
				}
				else{
					fprintf(stdout, "Usage: algo <fifo|sjf|balanced>\n");
				}
			}
		}
		else if(!strcmp(splits[0], "quit")){
			is_quit = 1;
		 
			delete_dll(job_queue);
			exit(0);
		}
		else if(!strcmp(splits[0], "help")){
			fprintf(stdout, "submit <file name>\nlist\nwait <job id>\ndrain\nremove <job id>\nhurry <job id>\nalgo <fifo|sjf|balanced>\nquit\n");
		}
		else{
			fprintf(stdout, "use \"help\" command\n");
		}
	}
	for(i = 0; i < NUM_THREADS; i++){
		pthread_join(tid[i], NULL);
	} 
	delete_dll(job_queue);
	return 0;
}






void *printer_handler(){
	while(1){	
		pthread_mutex_lock(&lock);
		while(num_jobs < 1){
			pthread_cond_wait(&cond, &lock);
			if(is_quit){
				return NULL;
			}
		}
		node_t *curr_node;
		if(algorithm == 0){
			curr_node = job_queue->head;
			while(curr_node != NULL && curr_node->status != WAITING){
				curr_node = curr_node->next;
			}
		}	
		else if(algorithm == 1){
			curr_node = smallest_job(); 
		}
		else{
			curr_node = smallest_job_with_wait_prio();
		}		
		num_jobs--;
		curr_node->time_started = time(NULL);
		curr_node->status = PRINTING;
		char *exec_args[2];			
		exec_args[0] = "./exe/printersim";
		exec_args[1] = curr_node->file_name;
	
		pthread_mutex_unlock(&lock);
	
		int pid = fork();
		if(pid == 0){
			if(execvp("./exe/printersim", exec_args) < 0){
				printf("error with execvp on line 231: %s\n", strerror(errno));
				pthread_mutex_lock(&lock);
				curr_node->status = FAILURE;
				pthread_mutex_unlock(&lock);
			}
		}	
		else{
			if(waitpid(pid, NULL, 0) < 0){
				printf("error with waitpid on line 240: %s\n", strerror(errno));
				exit(-1);
			}
			else{
				pthread_mutex_lock(&lock);
				curr_node->time_completed = time(NULL);	
				curr_node->status = DONE;
				pthread_cond_broadcast(&cond);
				pthread_mutex_unlock(&lock);
			}
		}
	}
	return NULL;
}	

void print_jobs(){
	pthread_mutex_lock(&lock);
	if (job_queue->head == NULL){
		printf("No current jobs\n");
		return;
	}
	int jobs_completed = 0;
	int total_turnaround_time = 0;
	int total_response_time = 0;
	node_t *temp = job_queue->head;
	while(temp != NULL){
		if (temp->status == WAITING){
			printf("job_id:%d | job_status: WAITING | file_name: %s\n", temp->job_id, temp->file_name);
		}
		else if (temp->status == PRINTING){
			printf("job_id:%d | job_status: PRINTING | file_name: %s\n", temp->job_id, temp->file_name);
		}	
		else if(temp->status == FAILURE){
			printf("job_id:%d | job_status: FAILURE!! | file_name: %s\n", temp->job_id, temp->file_name);
		}	
		else{	
			printf("job_id:%d | job_status: DONE | file_name: %s\n", temp->job_id, temp->file_name);
			jobs_completed++;
			total_turnaround_time += (temp->time_completed - temp->time_arrived);
			total_response_time += (temp->time_started - temp->time_arrived);
		}
		temp = temp->next;
	}
	double avg_response_time = (double)total_response_time/jobs_completed;
	double avg_turnaround_time = (double)total_turnaround_time/jobs_completed;
	printf("Average turnaround time: %lf\nAverage response time: %lf\n", avg_turnaround_time, avg_response_time);
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&lock);
	return;
}


int queue_wait(int job_id){

	pthread_mutex_lock(&lock);
	if (job_queue->head == NULL){
		printf("No current jobs\n");
		return 1;
	}

	node_t *temp = job_queue->head;
	while(temp != NULL && temp->job_id != job_id){
		temp = temp->next;
	}
	if(temp == NULL){
		pthread_mutex_unlock(&lock);
		printf("error: no matching job_id\n");
		return 1;
	}
	pthread_mutex_unlock(&lock);
	
	while(1){
		pthread_mutex_lock(&lock);
		if(temp->status == DONE || temp->status == FAILURE) break;
		pthread_mutex_unlock(&lock); // necessary
	}
	if(temp->status == FAILURE){
		fprintf(stdout,"FAILURE | Arrival Time: %d | Start Time: %d | Exit Time: %d\n", temp->time_arrived, temp->time_started, temp->time_completed);
	}
	else if(temp->status == DONE){
		fprintf(stdout,"SUCCESS | Arrival Time: %d | Start Time: %d | Completion Time: %d\n", temp->time_arrived, temp->time_started, temp->time_completed);
	}
	pthread_mutex_unlock(&lock);
	return 1;
}



int drain_queue(){
	if (job_queue->head == NULL){
		printf("No current jobs\n");
		return 1;
	}
	node_t *temp = job_queue->head;
	while(temp != NULL){
		pthread_mutex_lock(&lock);
		if (temp->status != DONE) return 0;
		temp = temp->next;
		pthread_mutex_unlock(&lock);
	}
	return 1;
}



node_t *smallest_job(){
	if (job_queue->head == NULL){
		return NULL;
	}

	node_t *temp = job_queue->head;
	node_t *res = temp;
	while(temp != NULL){
		if (temp->job_size < res->job_size && temp->status == WAITING){
			res = temp;
		}
		temp = temp->next;
	}
	return res;
}

node_t *smallest_job_with_wait_prio(){
	if (job_queue->head == NULL){
		return NULL;
	}

	node_t *temp = job_queue->head;
	node_t *res = temp;
	while(temp != NULL){
		if (time(NULL) - temp->time_arrived > 70){
			res = temp;
			break;
		}
		if ((temp->job_size < res->job_size) && (temp->status == WAITING)){
			res = temp;
		}
		if (time(NULL) - temp->time_arrived > 50){
			res = temp;
			break;
		}
		temp = temp->next;

	}
	return res;
}


