#include "dll.h" 


int push_back(dll_t *job_queue, node_t *inserted) {
	if(job_queue->head == NULL){
		job_queue->length++;
		job_queue->head = inserted;
		job_queue->tail = inserted;
		return 0;
	}
	else if(inserted == NULL || job_queue == NULL){
		return -1;
	}
	else{
		job_queue->length++;
		node_t *temp = job_queue->tail;
		temp->next = inserted;
		inserted->prev = job_queue->tail;
		job_queue->tail = inserted;
		return 0;
	}
}


node_t *create_node(int job_size, int job_id, int time_arrived, char *file_name){
	node_t *new_node = (node_t *)malloc(sizeof(node_t));
	new_node->job_size = job_size;
	new_node->job_id = job_id;
	new_node->file_name = (char *)malloc(sizeof(char)*strlen(file_name));
	strcpy(new_node->file_name, file_name);
	new_node->time_arrived = time_arrived;
	new_node->time_started = -1;
	new_node->time_completed = -1;
	new_node->status = WAITING;
	new_node->priority = 0;
	new_node->next = NULL;
	new_node->prev = NULL;
	return new_node;
	
}

int delete_dll(dll_t *job_queue){
	if(job_queue == NULL){
		return -1;
	}
	node_t *temp = job_queue->tail;
	node_t *temp2 = temp->prev;
	while(1){
		free(temp);
		if(temp2 == NULL) break;
		temp = temp2;
		temp2 = temp->prev;
	}
	free(job_queue);
	return 0;
}

