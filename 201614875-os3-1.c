#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>

/* macro and structure */
#define PAGESIZE (32)
#define PAS_FRAMES (256)
#define PAS_SIZE (PAGESIZE*PAS_FRAMES)
#define VAS_PAGES (64)
#define VAS_SIZE (PAGESIZE*VAS_PAGES)
#define PTE_SIZE (4)
#define PAGETABLE_FRAMES (VAS_PAGES*PTE_SIZE/PAGESIZE)

#define PAGE_INVALID (0)
#define PAGE_VALID (1)

#define MAX_REFERENCES (256)

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
	unsigned char f[PAGESIZE];
}frame;

process_raw **process;
int process_count = 0;

void memorySchedulingSimulator()
{
	int frame_num = 0;
	int totalrf = 0;
	int totalpf = 0;
	int *rf = (int *) calloc(process_count, sizeof(int));
	int *pf = (int *) calloc(process_count, sizeof(int));

	pte *cur_pte;
	frame *pas = (frame *) malloc(PAS_SIZE);

	/* pagetable allocation */
	for(int i = 0; i < process_count; i++)
	{
		for(int j = 0; j < PAGETABLE_FRAMES; j++)
		{
			cur_pte = (pte *)&pas[frame_num];

			for(int k = 0; k < VAS_PAGES/PAGETABLE_FRAMES; k++)
			{
				cur_pte->ref = 0;
				cur_pte->vflag = PAGE_INVALID;

				cur_pte += 1;
			}	
			
			frame_num += 1;
		}
	}
	
	/* page access */
	for(int j = 0; j < MAX_REFERENCES; j++)
	{
		for(int i = 0; i < process_count; i++)
		{
			if(process[i]->ref_len > j)
			{
				cur_pte = (pte *)&pas[i * PAGETABLE_FRAMES] + process[i]->references[j];
				
				if(cur_pte->vflag == PAGE_VALID)
				{
					cur_pte->ref += 1;
					rf[i] += 1;
				}

				else if(cur_pte->vflag == PAGE_INVALID)
				{
					if (frame_num >= PAS_FRAMES)
					{
						printf("Out of memory!!\n");
						goto Report;
					}

					cur_pte->frame = frame_num;
					frame_num += 1;

					cur_pte->vflag = PAGE_VALID;
					cur_pte->ref += 1;
					pf[i] += 1;
					rf[i] += 1;
				}
			}
		}
	}

Report:
	/* report */
	for(int i = 0; i < process_count; i++)
	{
		totalrf += rf[i];
		totalpf += pf[i];
		printf("** Process %03d: Allocated Frames=%03d PageFaults/References=%03d/%03d\n", i, pf[i] + PAGETABLE_FRAMES, pf[i], rf[i]);


		for(int j = 0; j < VAS_PAGES; j++)
		{
			cur_pte = (pte *)&pas[i * PAGETABLE_FRAMES] + j;

			if(cur_pte->vflag == PAGE_VALID)
				printf("%03d -> %03d REF=%03d\n", j, cur_pte->frame, cur_pte->ref);
		}
	}

	printf("Total: Allocated Frames=%03d Page Faults/References=%03d/%03d\n", totalpf + process_count * PAGETABLE_FRAMES, totalpf, totalrf);

	free(rf);
	free(pf);
}

int main(int argc, char* argv[])
{
	int i, j;
	process = malloc(sizeof(process_raw *) * 10);

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

			//printf("%d %d\n", process[i]->pid, process[i]->ref_len);

			process[i]->references = malloc(sizeof(unsigned char) * process[i]->ref_len);
			
			for(j = 0; j < process[i]->ref_len; j++)
			{
				if(fread(&process[i]->references[j], sizeof(unsigned char), 1, stdin) < 1)
					printf("error2\n");

					
				//printf("%d\n", process[i]->references[j]);
			}
		}
	}

	memorySchedulingSimulator();

	/* free memory allocation */
	for(i = 0; i < process_count; i++)
	{
		free(process[i]->references);
		free(process[i]);
	}

	free(process);

	return 0;
}