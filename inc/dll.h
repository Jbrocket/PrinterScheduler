#include <stdlib.h>
#include <string.h>

typedef enum{
	WAITING = 0,
	PRINTING = 1,
	DONE = 2,
	FAILURE = 3
} status_t;

typedef struct node{
	int job_size;
	int job_id;
	int time_arrived;
	int time_started;
	int time_completed;
	int priority;
	status_t status;
	char *file_name;
	struct node *prev;
	struct node *next;
} node_t;

typedef struct {
	struct node *head;
	struct node *tail;
	int length;		
} dll_t;

int push_back(dll_t *job_queue, node_t *inserted);
node_t *create_node(int job_size, int job_id, int time_arrived, char *file_name);
int delete_dll(dll_t *job_queue);

