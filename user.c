#include "circuit.h"
int counter = 0;
int no_of_gates (NODE *graph, int max_node_id){
	int i;
	int counter=0;
//this is because the gates are not filled sequentially
	for (i=0;i<=max_node_id; i++){
		if (graph[i].type !=0)
			counter++;
	}
	return counter;
} 


// Create the cell arrays and net arrays
void adj_func(NODE *graph, CELL *noc, NET *con, int max_node_id, int counter){
	int n,i,j=0;
	LIST *temp_list;
	//assign BLK values i.e Cell array
	for (n=0; n<=max_node_id ; n++)	{	
		if (graph[n].type !=0){
			noc[n].isLocked = 0;
			if (j<(int)(counter/2)+counter%2)
				noc[n].BLK = 0; 
			else
				noc[n].BLK = 1;
			j++;
		}		
	}//assign BLK ends

//stores the ID in cell array and net array
	for(n=0; n<=max_node_id; n++){   //for loop for visiting nets
		if(graph[n].type!=0){				
			temp_list=graph[n].fanout;
			while(temp_list!=NULL){
				insert_list(&(con[n].cells),temp_list->ID);
				insert_list(&(noc[temp_list->ID].nets),n);
				temp_list=temp_list->next;
			}
			if(graph[n].fanout!=NULL){
				insert_list(&(con[n].cells),n);
				insert_list(&(noc[n].nets),n);
			}
		}
 	}
	
	
//creating the NET array
	for (n = 0; n<= max_node_id; n++){
		if (graph[n].type!= 0){ // ignore the empry cells in the list 
			temp_list=con[n].cells; //assign ID of cell list of net 'n'
			while(temp_list != NULL){
				if (noc[temp_list->ID].BLK == 0) //looks at the BLK value of cells in the cell list  
						con[n].Na++;
				else
					con[n].Nb++;
				temp_list = temp_list->next;		
			}
		}
	}
}
	

//PRINT CELL ARRAY
void print_noc(NODE graph[MAX_NODES],CELL noc[MAX_NODES],int max_node_id){
	int i;
	printf  	("\nCELL ARRAY\n");
	printf("ID\tNet List\tBlk\t\tGain\t\tLock\n");
	for(i=0; i<=max_node_id; i++){
		if (graph[i].type !=0){
			printf("%d\t",i);
			print_list(noc[i].nets);
			printf("\t\t%d",noc[i].BLK);
			printf("\t\t%d", noc[i].gains);
			printf("\t\t%d\n", noc[i].isLocked); 
		}
	}
}


//PRINT NET ARRAY
void print_con(NODE graph[MAX_NODES],NET con[MAX_NODES],int max_node_id){
	int i;
	printf("\nNET ARRAY\n");
	printf("ID\tCell List\tNa\tNb\n");
	for(i=0; i<=max_node_id; i++){
		if (graph[i].type !=0){
			printf("%d\t",i);
			print_list(con[i].cells);
			printf("\t\t%d\t%d\n",con[i].Na,con[i].Nb);
		}
	}
	printf("\n");
}

// Pmax calculation function
int Pmax_calculator (NODE *graph, CELL *noc, int max_node_id){
	int i, count = 0, list_counter;
	for (i=0; i<=max_node_id; i++){
		if ( graph[i].type != 0){
			list_counter = count_list(noc[i].nets);
			if (count < list_counter){
				count = list_counter;
			}
		}
	}
	return count;
}


//GAIN CALCULATION
void gain_calculator (NODE graph[MAX_NODES], CELL noc[MAX_NODES], int max_node_id, NET con[MAX_NODES]){
	int c, F, Fn, T, Tn, gain,n ;
	LIST *temp_list;
 
	for (c = 0; c<= max_node_id; c++){
 		if (graph[c].type !=0){
			temp_list = noc[c].nets;
						noc[c].gains = 0;
			gain = 0; //gain of every cell is initialized to zero initially
			F = noc[c].BLK; //current block of cell[c]
			T = 1-F; // complementary block of cell[c]
			
			while(temp_list != NULL){
				n = temp_list->ID; //n holds the ID of the cell 
				if (F == 0){
					Fn = con[n].Na; 
					Tn = con[n].Nb;			
				}
				else{
					Fn = con[n].Nb;
					Tn = con[n].Na;			
				}
				
				if (Fn == 1)	gain++;
				if (Tn == 0)	gain--;
				
				noc[c].gains = gain;	
				temp_list= temp_list->next;	
			}
		}
	}
}




//creating the bucketlist
void bucket_list (NODE *graph, CELL *noc, int max_node_id, DLIST *bktA, DLIST *bktB, int p_max, int *maxgainA, int *maxgainB){
	int gain, c;
	*maxgainA = *maxgainB = 0;
	DLIST *temp_dlist;
	for ( c = 0; c<= max_node_id; c++){
			if (graph[c].type != 0){
			 gain = noc[c].gains;
			 if (noc[c].BLK ==0){
			 if (*maxgainA < (gain+p_max))
				 *maxgainA = gain+p_max;
			 insert_dlist (&(bktA[gain+p_max].next), c);
			 }
			 else{
			 if (*maxgainB < (gain+p_max))
				 *maxgainB = gain+p_max;
			 insert_dlist (&(bktB[gain+p_max].next), c);
			}
		}
	}
}




void bucket_list_initialization (DLIST *bktA, DLIST *bktB, int p_max){
	int i;
	//initialize bucket list
	for ( i = 0; i< (2*p_max+1); i++){
		bktA[i].next = bktA[i].prev = NULL;
		bktA[i].ID = 0;
		bktB[i].next = bktB[i].prev = NULL;
		bktB[i].ID = 0;
	}  
}


void print_bucket_list (DLIST *bktA, DLIST *bktB, int p_max){
	int i;
	printf("Bucket List A\n");
	printf("Index\tGain\tCells\n");
	for ( i = 0; i< (2*p_max+1); i++){
		printf("%d\t%d\t",i, i-p_max);
		print_dlist(bktA[i].next);
	//printf("\tcount %d",count_dlist(bktA[i].next)); 
		printf("\n");
	}  
	printf("Bucket List B\n");
	printf("Index\tGain\tCells\n");
	for ( i = 0; i< (2*p_max+1); i++){
		printf("%d\t%d\t",i, i-p_max);
		print_dlist(bktB[i].next);
//printf("\tcount %d",count_dlist(bktB[i].next));
		printf("\n");
	}  
	
}


int cell(DLIST *bktA, DLIST *bktB, CELL *noc, NODE *graph, int max_node_id, int *maxgainA, int *maxgainB, int *sizeA, int *sizeB){
	int i,base_cell, id;
	if (*sizeA>*sizeB){
		base_cell =bktA[*maxgainA].next->ID; //id = id of the cell that is being moved.
		*sizeA= *sizeA - 1;
		*sizeB= *sizeB + 1;
	}
	else if (*sizeB>*sizeA){
			base_cell =bktB[*maxgainB].next->ID;
			*sizeB= *sizeB - 1;
			*sizeA= *sizeA + 1;
		}
		else{   //if both partitions are equal slection is done based on the max gain
			if ( *maxgainA >= *maxgainB){
				base_cell =bktA[*maxgainA].next->ID;
				*sizeA= *sizeA - 1;
				*sizeB= *sizeB + 1;
			} 
			else{
				base_cell =bktB[*maxgainB].next->ID;
				*sizeB= *sizeB - 1;
				*sizeA= *sizeA + 1;
		}
	}
	return base_cell;
}


void gain_update (int base_cell, NODE *graph, CELL *noc, NET *con, DLIST *bktA, DLIST *bktB, int p_max, int *cut, int *maxgainA, int *maxgainB){
	int F, Fn, T, Tn,n, cell, id;
	int initial_gain; // used to store the gain of cell before the update to locate it in the bucket list
	LIST *temp_noc;
	LIST *temp_con;
	cell = base_cell;
	temp_noc = noc[cell].nets; //assign ID of netlist of  cell "cell"
	F = noc[cell].BLK; 
	T = 1-F; 
	noc[cell].isLocked = 1; // lock the base cell
	noc[cell].BLK = T; //compliment the block
 		
	while(temp_noc != NULL){
		n = temp_noc->ID; //the net list of the base cell
		if (F == 0){
			Fn = con[n].Na; 
			Tn = con[n].Nb;
		}
		else{
			Fn = con[n].Nb;
			Tn = con[n].Na;
		}
		// Check critical nets before the move
		temp_con = con[n].cells; //cell list of net n
		if (Tn == 0 ){ 
			//this loop goes through the all the free cells of net "n"
			while (temp_con != NULL){
				id = temp_con->ID;
				if ( noc[id].isLocked == 0){
					initial_gain = noc[id].gains;
					noc[id].gains++;
					bkt_update (id, initial_gain, noc[id].gains,0, bktA, bktB, p_max, noc, maxgainA, maxgainB);
				}
				temp_con = temp_con->next; //the cell list of all nets connected to the base cell
			}
		}
		else if ( Tn == 1 ){
			while (temp_con != NULL){
				id = temp_con->ID;
				if ( noc[temp_con->ID].isLocked == 0 && noc[temp_con->ID].BLK == T ){ 
						initial_gain = noc[temp_con->ID].gains;
						noc[temp_con->ID].gains--;
						bkt_update (id, initial_gain, noc[id].gains,0, bktA, bktB, p_max, noc, maxgainA, maxgainB);
					}
				temp_con = temp_con->next;
				}
		}
		
		//Change the net distribution to reflect the move
		if (F == 0){ 
			con[n].Na--; 
			con[n].Nb++;
		}
		else{
			con[n].Nb--; 
			con[n].Na++;
		}
		
		//Check critical nets after the move
		if (Fn == 0 ){ 
			//this loop goes through the all the free cells of net "n"
			while (temp_con != NULL){
					if ( noc[temp_con->ID].isLocked == 0){
							initial_gain = noc[temp_con->ID].gains;
							noc[temp_con->ID].gains--;
							bkt_update (id, initial_gain, noc[id].gains,0, bktA, bktB, p_max, noc, maxgainA, maxgainB);
						}
					temp_con = temp_con->next;
				}
			}
			
		else if ( Fn == 1 ){
			while (temp_con != NULL){
				if ( noc[temp_con->ID].isLocked == 0 && noc[temp_con->ID].BLK == F ){
					initial_gain = noc[temp_con->ID].gains;
					noc[temp_con->ID].gains++;
					bkt_update (id, initial_gain, noc[id].gains,0, bktA, bktB, p_max, noc, maxgainA, maxgainB);
				}
				temp_con = temp_con->next;
			}
		}
		temp_noc = temp_noc->next;      
	}	
}



void initialize_cutset (NET *con, int max_node_id, int *cut){
	int i;
	*cut = 0;
	for ( i = 0; i<= max_node_id; i++){
		if (con[i].Na > 0 && con[i].Nb > 0) 
			*cut += 1;
	}
}

// Insert D-List
void insert_dlist (DLIST ** head, int id){
	 DLIST *temp,*next;	 
	 if((temp = (DLIST*)malloc(sizeof(DLIST)))==NULL){
		 printf("INSER_LIST out of memory\n");
		 exit(1);
	 }
	 temp->ID=id;
	 temp->next=NULL;
	 temp->prev=NULL;
	 if(*head==NULL)
	 	*head=temp;
	 else{
			next = *head;
			while(next->next!=NULL){
		 		next=next->next;
			}
		 	next->next=temp;
		 	temp->prev = next;
	 } 		 
}



void delete_dlist (DLIST **bkt, int gain, int cell, int p_max){		//cell to be deleted and gain of the cell
 DLIST *temp;
 temp = *bkt;
 while (temp != NULL){
	 if (temp->ID ==cell){
			if (temp->prev != NULL)
			 temp->prev->next = temp->next;
		 else 
			 *bkt = temp->next;
			if (temp->next != NULL)
			 temp->next->prev = temp-> prev;
			free (temp);
			break;
	 }
	 temp =temp->next;
	}
}



//function to update the bucket list after a cell move
void bkt_update (int cell, int initial_gain, int final_gain, int is_basecell, DLIST *bktA, DLIST *bktB, int p_max, CELL *noc, int *maxgainA, int *maxgainB){
	int n;
	DLIST *temp;
	if (noc[cell].BLK == 0){
		delete_dlist (&(bktA[initial_gain+p_max].next), initial_gain, cell, p_max);
		if (!is_basecell){
			insert_dlist(&(bktA[final_gain+p_max].next), cell);
		}
		if (*maxgainA < (final_gain+p_max))
			*maxgainA = final_gain+p_max;
		else
			if (bktA[*maxgainA].next == NULL)
				while (bktA[*maxgainA].next == NULL)
					*maxgainA = *maxgainA -1;
	}
	if (noc[cell].BLK == 1){	 
		delete_dlist (&(bktB[initial_gain+p_max].next), initial_gain, cell, p_max);
		if(!is_basecell)
		insert_dlist(&(bktB[final_gain+p_max].next), cell);
		if (*maxgainB < (final_gain+p_max))
			*maxgainB = final_gain+p_max;
		else
			if (bktB[*maxgainB].next ==NULL)
				while (bktB[*maxgainB].next == NULL)
					*maxgainB = *maxgainB-1;
	}
}


//function to reset the cells to default place after getting the best cut for a pass

void reset (FCLIST *free_cells, NET *con, CELL *noc, int *best_cut, NODE *graph, int max_node_id, int *sizeA, int *sizeB){
	FCLIST *temp, *temp2;
	LIST *temp_con;
	int i;
	temp = free_cells;
	while (temp != NULL){
		temp2 = temp->next;
		noc[temp[0].ID].isLocked = 0;
		if (temp[0].cut_set == *best_cut && temp2[0].cut_set != *best_cut){
			temp = temp->next;
			break;
		}
		temp = temp->next;
	}
	while (temp != NULL){ 
		noc[temp[0].ID].isLocked = 0;
		noc[temp[0].ID].BLK = 1 - noc[temp[0].ID].BLK;
		if (noc[temp[0].ID].BLK == 0){
			*sizeB= *sizeB - 1;
			*sizeA= *sizeA + 1; 
		}
		else{
			*sizeB= *sizeB + 1;
			*sizeA= *sizeA - 1; 
		}
		temp = temp->next;
	}
	free(temp);
}

void reset_all (FCLIST *free_cells,CELL *noc, int *sizeA, int *sizeB){
	FCLIST *temp;
	LIST *temp_con;
	int i;
	temp = free_cells;
	while (temp != NULL){ 
		noc[temp[0].ID].isLocked = 0;
		noc[temp[0].ID].BLK = 1 - noc[temp[0].ID].BLK;
		if (noc[temp[0].ID].BLK == 0){
			*sizeB= *sizeB - 1;
			*sizeA= *sizeA + 1; 
		}
		else{
			*sizeB= *sizeB + 1;
			*sizeA= *sizeA - 1; 
		}
		temp = temp->next;
	}
	free(temp);
}


void update_net_array (NODE *graph, NET *con, CELL *noc, int max_node_id){
	LIST *temp_list;
	int n;
	for (n = 0; n<= max_node_id; n++){	// goes through gates 1 through max_node_id
		con[n].Na =0;
		con[n].Nb =0;
		if (graph[n].type!= 0){	// ignore the empry cells in the list 
			temp_list=con[n].cells; //assign ID of cell list of net 'n'
			while(temp_list != NULL){
				if (noc[temp_list->ID].BLK == 0) //looks at the BLK value of cells in the cell list  
										con[n].Na++;
				else
					con[n].Nb++;
				temp_list = temp_list->next;		
			}
		}
	}
}