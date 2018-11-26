#include "ll_utils.h"
#include "MyPinToolUtils.h"
/*
 * adds a new node (job_queue_node_struct struct) TO THE END of a linked list pointed to by head
 */
int add_to_effected(race_issues **head,void* mem_addr, THREADID tid1, THREADID tid2)
{
    /* allocate memory for a single background process node */
    race_issues* new_issue = (race_issues*)malloc (sizeof(race_issues));
    if(new_issue == NULL)
    { // == -1
        /* return -1 if malloc fails */
        perror("error, malloc for new job queue node struct failed");
        return -1;
    }

    new_issue->threadid_1 = tid1;
    new_issue->threadid_2 = tid2;
    new_issue->effected_addr = (ADDRINT*)mem_addr;


    if((*head) == NULL)
    {
        /* if head == NULL, node created is the new head of the list! */
        *head = new_issue;
        printf("adding to head\n");
    } 
    else{
        /* otherwise, traverse linked list until we reach the end */
        race_issues* current = *head;

        while(current->next != NULL)
        {
            current = current->next;
        }
        /* add node to the end of list */
        current->next = new_issue;

    }
	return 0 ;
}




 /* Given a reference (pointer to pointer) to the head of a list
   and a key, deletes the first occurrence of key in linked list */
int remove_from_job_queue(race_issues **head, ADDRINT* mem_addr){
    // Store head node
    race_issues* temp = *head;
    race_issues* prev;

    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->effected_addr == mem_addr){
        *head = temp->next;       // Changed head
        free(temp);               // free old head
        return 0;
    }

    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && temp->effected_addr != mem_addr){
        prev = temp;
        temp = temp->next;
    }

    // If key was not present in linked list
    if (temp == NULL){
       return 0;
     }
    // Unlink the node from linked list
    prev->next = temp->next;
    free(temp);  // Free memory
    return 0;
}



/* Given a reference (pointer to pointer) to the head of a list
 returns the ID of the last node in the queue */
int contains(race_issues **head, ADDRINT* mem_addr){

     // Store head node
     race_issues* temp = *head;

     if (temp == NULL) return 0; // If key was not present in linked list

     // Search for the key to be deleted, keep track of the
     // previous node as we need to change 'prev->next'
     while (temp != NULL){
        if(mem_addr == temp->effected_addr) return 1;
         temp = temp->next;
     }
     // Unlink the node from linked list

     return 0;
  }

/*
 * print entire background process linked list
 */
void log_issue_queue(race_issues** head,std::ostream* fp)
{

  race_issues* current = *head;

  // make sure head isn't NULL
  if(current == NULL){
    //printf("issue list empty\n");
    return;
  }
  //printf("------ printing queue ------\n");
  while(current != NULL){
    //printf("[%d] [%d] [%p] \n", current->threadid_1, current->threadid_2, current->effected_addr);
    *fp << current->effected_addr << endl;
    current = current->next;
  }
  // printf("----- end queue -----\n");
  return;
}


/*
 * print entire background process linked list
 */
void print_issue_queue(race_issues** head){

  race_issues* current = *head;

	// make sure head isn't NULL
  if(current == NULL){
    printf("issue list empty\n");
    return;
  }
  printf("------ printing queue ------\n");
  while(current != NULL){
    printf("[%d] [%d] [%p] \n", current->threadid_1, current->threadid_2, current->effected_addr);
    current = current->next;
  }
  printf("----- end queue -----\n");
	return;
}


/*
 * delete entire job queue linked list
 */
int delete_job_queue (race_issues** head){

    if(*head == NULL) return 0; // list is empty, nothing to delete

	  // delete entire list node by node
    race_issues* temp_issue = *head;
    race_issues* next_issue = NULL;

    while(temp_issue != NULL){
        next_issue = temp_issue->next;
        free(temp_issue);
        temp_issue = next_issue;
    }
	  // if no errors, set head = NULL upon deletion
    *head = NULL;

	  // return 0 if no error, -1 for any errors that may arise
	  return 0 ;
}

/* Given a reference (pointer to pointer) to the head of a list
 returns true or false if the element exists */ 
int contains_event(event_list **head, ADDRINT mem_addr){

     // Store head node
     event_list* temp = *head;

     if (temp == NULL) return 0; // If key was not present in linked list

     // Search for the key to be deleted, keep track of the
     // previous node as we need to change 'prev->next'
     while (temp != NULL){
        if(mem_addr == temp->address) return 1;
         temp = temp->next;
     }

     return 0;
}

/*
 * adds a new node TO THE END of a linked list pointed to by head
 */
int add_to_event(event_list **head,ADDRINT mem_addr)
{
    /* allocate memory for a single background process node */
    event_list* new_event = (event_list*)malloc (sizeof(event_list));
    if(new_event == NULL)
    { // == -1
        /* return -1 if malloc fails */
        perror("error, malloc for new job queue node struct failed");
        return -1;
    }

    new_event->address = mem_addr;


    if((*head) == NULL)
    {
        /* if head == NULL, node created is the new head of the list! */
        *head = new_event;
        printf("adding to head\n");
    } 
    else{
        /* otherwise, traverse linked list until we reach the end */
        event_list* current = *head;

        while(current->next != NULL)
        {
            current = current->next;
        }
        /* add node to the end of list */
        current->next = new_event;

    }
  return 0 ;
}

/*
 * print entire background process linked list
 */
void print_event_queue(event_list** head){

  event_list* current = *head;

  // make sure head isn't NULL
  if(current == NULL){
    printf("issue list empty\n");
    return;
  }
  printf("------ printing events ------\n");
  while(current != NULL){
    printf("[%ld]\n", current->address);
    current = current->next;
  }
  printf("----- end events -----\n");
  return;
}


/*
 * delete entire event queue linked list
 */
int delete_event_queue (event_list** head){

    if(*head == NULL) return 0; // list is empty, nothing to delete

    // delete entire list node by node
    event_list* temp_issue = *head;
    event_list* next_issue = NULL;

    while(temp_issue != NULL){
        next_issue = temp_issue->next;
        free(temp_issue);
        temp_issue = next_issue;
    }
    // if no errors, set head = NULL upon deletion
    *head = NULL;

    // return 0 if no error, -1 for any errors that may arise
    return 0 ;
}