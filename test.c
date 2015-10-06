#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
struct prinfo {
	long state;			/* current state of process */
	pid_t pid;			/* process id */
	pid_t parent_pid;		/* process id of parent */
	pid_t first_child_pid;  	/* pid of youngest child */
	pid_t next_sibling_pid;  	/* pid of older sibling */
	long uid;			/* user id of process owner */
	char comm[64];			/* name of program executed */
};

#define __NR_ptree 223
#define INIT_BUF_SIZE 10 

static int find_parent(int *stack, int top, int parent)
{
	while (top >= 0) {
		if (stack[top] == parent)
			break;
		top--;
	}

	return top;
}

int main()
{
    // TODO: Implement
	struct prinfo *buf = NULL;
	int size = INIT_BUF_SIZE;
	int ret = 0;
	int i, top;

	do {
		if (buf) {
			size = ret + 10;
			free(buf);
		}

		buf = malloc(size * sizeof(struct prinfo));
		if (!buf) {
			printf("No memory!\n");
			exit(1);
		}
		ret = syscall(__NR_ptree, buf, &size);
		if (ret < 0) {
			perror("error: ");
			exit(1);
		}
	} while (ret > size);

	int *stack = malloc(size * sizeof(int));
	if (!stack) {
		printf("No memory!\n");
		exit(1);
	}

	stack[0] = 0;
	top = 0;
	for (i = 0; i < size; i++) {
		struct prinfo *p = &buf[i];
		int p_pos = find_parent(stack, top, p->parent_pid);
		if (p_pos < 0)
			stack[++top] = p->parent_pid;
		else
			top = p_pos;

		int j;
		for (j = 0; j < top; j++)
			printf("\t");

		printf("%s,%d,%ld,%d,%d,%d,%ld\n", p->comm, p->pid, p->state,
			p->parent_pid, p->first_child_pid, 
			p->next_sibling_pid, p->uid);
	}
	
	free(buf);
	free(stack);
	return 0;
 }
