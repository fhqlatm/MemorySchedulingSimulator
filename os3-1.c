#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>

/* macro and structure */
#define PAGESIZE (32)
#define PAS_FRAME (256)
#define PAS_SIZE (PAGESIZE*PAS_FRAMES)
#define VAS_PAGES (64)
#define VAS_SIZE (PAGESIZE*VAS_PAGES)
#define PTE_SIZE (4)
#define PAGETABLE_FRAMES (VAS_PAGES*PTE_SIZE/PAGESIZE)

#define MAX_REFERENCES (256)
#define PAGE_INVALID (0)
#define PAGE_VALID (1)

typedef struct
{
	unsigned char frame;
	unsigned char vflag;
	unsigned char ref;
	unsigned char pad;
}pte;

typedef struct
{
	int pid;
	int ref_len;
	unsigned char *references;
}process_raw;

typedef struct
{
	int pid;
	int ref_len;
}process_t;


typedef struct
{
	unsigned char frame[PAGESIZE];
}frame;

/* assignment */
void memorySchedulingSimulator()
{

}

int main(int argc, char* argv[])
{
	int i, j;
	int process_count = 0;
	process_raw **process = malloc(sizeof(process_raw *) * 10);

	for(i = 0; i < 10; i++)
	{
		process[i] = malloc(sizeof(process_raw));
		
		if(fread(&process[i]->pid, sizeof(int), 1, stdin) < 1)
		{
			free(process[i]);
			break;
		}

		else
		{	
			process_count += 1;

			if(fread(&process[i]->ref_len, sizeof(int), 1, stdin) < 1)
				printf("error1\n");

			printf("%d %d\n", process[i]->pid, process[i]->ref_len);

			process[i]->references = malloc(sizeof(unsigned char) * process[i]->ref_len);
			
			for(j = 0; j < process[i]->ref_len; j++)
			{
				if(fread(&process[i]->references[j], sizeof(unsigned char), 1, stdin) < 1)
					printf("error2\n");

					
				printf("%d\n", process[i]->references[j]);
			}
		}

	}

	memorySchedulingSimulator();

	/* free memory allocation */
	for(i = 0; i < process_count; i++)
	{
		for(j = 0; j < process[i]->ref_len; j++)
		{
			free(process[i]->references);
		}

		free(process[i]);
	}

	free(process);

	return 0;
}