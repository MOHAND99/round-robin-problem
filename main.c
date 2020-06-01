#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NAME_SIZE 4
#define F_LINE 32
#define P_LINE 16
/*
 * Process: Each process has a name and has an execution time.
 *          We keep track of how long the process still needs
 *          to run on the processor. "remaining_time" is the
 *          remaining execution time of the process.
 */
typedef struct
{
    char* name;
    int starting_time;
    int remaining_time;
} Process;
/*
 *
 */
typedef struct Node
{
    Process data;
    struct Node* next;
} Node;
/*
 *
 */
typedef struct
{
    Node *head, *tail;
} Queue;
/*
 *
 */
Queue* init()
{
    Queue* q = malloc(sizeof(Queue));
    q->head = q->tail = NULL;
    return q;
}
/*
 *
 */
int isEmpty(Queue *q)
{
    return q->head == NULL;
}
/*
 *
 */
Process dequeue(Queue *q)
{
    Process dequeueVal;
    dequeueVal.name = malloc(NAME_SIZE);
    dequeueVal = q->head->data;
    Node* temp = q->head;
    q->head = q->head->next;
    if(!q->head) q->tail = NULL;
    free(temp);
    return dequeueVal;
}
/*
 *
 */
Node* newNode(Process val){
    Node *n = malloc(sizeof(Node));
    n->data.name = malloc(NAME_SIZE);
    n->data = val; n->next =NULL;
    return n;
}
/*
 *
 */
Process getHeadData(Queue* q){ return q->head->data; }
/*
 *
 */
void enqueue(Queue *q, Process val)
{
    Node* n = newNode(val);
    if(q->tail) q->tail->next = n;
    if(!q->head) q->head = n;
    q->tail = n;
}
/*
 *
 */
//Process getTail(Queue* q) {return q->tail->data; }
void destroy(Queue *q)
{
    while(!isEmpty(q)) dequeue(q);
    free(q);
}
/*
 * get Watch Slots
 */
int getWatchSlot(char Line[])
{
    char *token;
    token = strtok(Line, " ");
    while(token)
    {
        if(atoi(token))
            return atoi(token);
        token = strtok(NULL, " ");
    }
    return -1;
}
/*
 * RoundRobin Scheduling
 */
void RoundRobin(char* filename)
{
    Queue* qProcs = init();
    Queue* qWait = init();
    FILE* file = fopen(filename,"r");
    char line[F_LINE];
    fgets(line, F_LINE, file);
    int tSlots = getWatchSlot(line);
    int tSlotNow = 0;
    while(fgets(line, P_LINE, file)){
        Process p;
        p.name = malloc(NAME_SIZE);
        sscanf(line, "%s %d %d", p.name, &p.starting_time, &p.remaining_time);
        enqueue(qWait, p);
    }
    while(!isEmpty(qWait) && getHeadData(qWait).starting_time == tSlotNow) {enqueue(qProcs, dequeue(qWait));}
    while(tSlotNow < tSlots){
        char isidle = 1;
        if(!isEmpty(qProcs)){
            isidle = 0;
            Process p;
            p.name = malloc(NAME_SIZE);
            p = getHeadData(qProcs);
            if(--p.remaining_time){
                while(!isEmpty(qWait) && getHeadData(qWait).starting_time == tSlotNow+1) enqueue(qProcs, dequeue(qWait));
                printf("%s   (%d-->%d)\n", getHeadData(qProcs).name, tSlotNow, tSlotNow+1); dequeue(qProcs);
                enqueue(qProcs, p);
            }else {
                printf("%s   (%d-->%d)  %s aborts\n", getHeadData(qProcs).name, tSlotNow, tSlotNow+1, getHeadData(qProcs).name);
                dequeue(qProcs);
                while(!isEmpty(qWait) && getHeadData(qWait).starting_time == tSlotNow+1) enqueue(qProcs, dequeue(qWait));
            }
        }
        else if(!isEmpty(qWait) && (getHeadData(qWait).starting_time == tSlotNow+1)){
            do enqueue(qProcs, dequeue(qWait));
            while(!isEmpty(qWait) && getHeadData(qWait).starting_time == tSlotNow+1);
        }
        if(isidle) printf("idle (%d-->%d)\n", tSlotNow, tSlotNow+1);

        tSlotNow++;
    }
    printf("stop\n");
    destroy(qWait); destroy(qProcs);
}
/*
 *
 */
int main()
{
    char filename[261];
    puts("Enter file name or Ctrl+Z to exit:");
    puts("----------------------------------");
    while(fgets(filename, 260, stdin) != NULL)
    {
        filename[strlen(filename)-1]='\0';
        if(fopen(filename,"r"))
            RoundRobin(filename);
        else
        {
            puts("File Not Found!");
            puts("----------------------------------");
        }
        puts("Enter file name or Ctrl+Z to exit:");
        puts("----------------------------------");
    }

    return 0;
}