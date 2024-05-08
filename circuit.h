#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include <time.h>

 /* NODE TYPE CONSTANTS */
#define INPT	1				/* Primary Input*/
#define AND   	2				/* AND       */
#define NAND 	3				/* NAND      */
#define OR  	4				/* OR        */
#define NOR 	5				/* NOR       */
#define NOT  	6				/* INVERTER  */
#define OUTPUT  7				/* OUTPUT    */ 
#define BUFF    8				/* BUFFER    */
#define MAX_NODES   25000

typedef struct list
{
   int ID;
   struct list *next;
} LIST;



typedef struct node
{
  int type;			//Gate Type
  int fanout_num;	//Number of fan-outs from a gate
  int fanin_num;	// Number of fan-ins to a gate
  int output;
  LIST *fanin;		//pointer to Linked list which represent the Fan-ins to a gate.
  LIST *fanout;	//pointer to Linked list which represent the Fan-outs from a gate.
 
}NODE;

/*-----------------------------------------------------------------
                  New Structures
------------------------------------------------------------------*/   

typedef struct cell
{
LIST *nets;//to keep list of nets connected to cell.
int BLK; //0 for Part. A 1 for part B
int gains; //for storing the gains of the cell.
int isLocked; //to lock the cell. O is unlocked 1 is locked.

}CELL;

typedef struct net
{
LIST *cells; //to keep list of cells connected to net
int Nb,Na; //Nb is no of cell connected to net from part. B

}NET;


/*-----------------------------------------------------------------/
      Doubly linked list. (Used to create the bucket lists.)
------------------------------------------------------------------*/   


typedef struct dlist
{
  int ID;
  struct dlist *next;
  struct dlist *prev;
} DLIST;

/*-----------------------------------------------------------------/
      FREE CELL LIST
------------------------------------------------------------------*/
typedef struct fclist //free cell list
{
  int ID;
  int cut_set;
  struct fclist *next;
  struct fclist *prev;
  
} FCLIST;




/*---------------------------------------------------------------------------
 			Function Prototypes                                                       
---------------------------------------------------------------------------*/

int read_circuit(FILE*,NODE[MAX_NODES],LIST**,LIST**);
void insert_list(LIST **, int);  
void initialize_graph(NODE *); 
void charcat(char * ,char );  
int assign_type(char *);
void print_graph(NODE [MAX_NODES]);
void print_list(LIST *);
int count_list(LIST *); 
void print_fclist(FCLIST *); 



/*-----------------------------------------------------------------
                  New Function Prototype
------------------------------------------------------------------*/   

void adj_func( NODE[MAX_NODES], CELL[MAX_NODES], NET[MAX_NODES], int, int);
void print_noc(NODE[MAX_NODES] , CELL [MAX_NODES], int);
void print_con(NODE[MAX_NODES] , NET [MAX_NODES], int);
int no_of_gates (NODE[MAX_NODES],int); 
void gain_calculator (NODE [MAX_NODES], CELL [MAX_NODES], int , NET [MAX_NODES]);
int Pmax_calculator (NODE [MAX_NODES], CELL [MAX_NODES], int );
void insert_dlist (DLIST **, int );
void bucket_list (NODE *, CELL *, int , DLIST *, DLIST *, int, int *, int *);
void print_dlist(DLIST *);
void print_bucket_list (DLIST *, DLIST *, int );
int cell(DLIST *, DLIST *, CELL *, NODE *, int , int *, int *, int *, int *);
void gain_update (int , NODE *, CELL *, NET *, DLIST *, DLIST *, int, int *, int *, int*); 
void insert_fclist(FCLIST ** , int , int * );
void initialize_cutset (NET *, int, int * );
void bkt_update (int, int , int , int, DLIST *, DLIST *, int, CELL *, int *, int *);
void delete_dlist (DLIST **, int , int , int );
void reset (FCLIST *, NET *, CELL *, int *, NODE *, int, int *, int *);
void update_net_array (NODE *, NET *, CELL *, int );
int count_dlist(DLIST *); 
void reset_all (FCLIST *,CELL *, int *, int *);
void bucket_list_initialization (DLIST *,DLIST *, int);










 

