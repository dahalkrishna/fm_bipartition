#include "circuit.h"

int main(int argc, char *argv[]){
	clock_t start, end;
	start = clock();
  FILE	*fp_bench;
  NODE	graph[MAX_NODES];

	CELL noc[MAX_NODES]; //noc: nets of cells ; refer to header file
	NET	con[MAX_NODES]; //con: cells of nets; refer to header file
  LIST	*in=NULL, *out=NULL;
  FCLIST *free_cells=NULL;
	int	max_node_id=0;
	int	counter, initial_gain, flag;
	int	p_max, maxgainA, maxgainB, best_cut_new,best_cut_old, i, base_cell, gain, index, sizeA = 0, sizeB = 0, pass =0 ;
	int id; // used to store the ID of the cell beign moved
	int cut; // the number of net cut   
	if(argc!=2){
     	printf("Insufficient arguments\n");
     	exit(0);
  	}
  
	fp_bench=fopen(argv[1],"r");
	max_node_id = read_circuit(fp_bench, graph, &in, &out);
  float enter;
  start = clock();
	counter = no_of_gates (graph, max_node_id);
	adj_func( graph, noc, con, max_node_id, counter );
	gain_calculator (graph, noc, max_node_id,  con);
  initialize_cutset (con, max_node_id, &cut);
  best_cut_new = best_cut_old = cut;
  p_max = Pmax_calculator (graph, noc, max_node_id);
  DLIST bktA[2*p_max+1], bktB[2*p_max+1];
  bucket_list_initialization (bktA, bktB, p_max);
  bucket_list (graph, noc, max_node_id, bktA, bktB, p_max, &maxgainA, &maxgainB);
// print_graph(graph);

//print_noc(graph, noc, max_node_id);
//print_con(graph, con, max_node_id);
//getchar();
  //loop to calculate the size of partition A and B
  
  for (i = 0; i<=max_node_id;i++){
    if (graph[i].type!= 0){
      if (noc[i].BLK == 0)
        sizeA++;
      else
        sizeB++;      
    }
  }
  
  //print_noc(graph, noc, max_node_id);
  //getchar();
  printf("Initial Cut = %d\n", cut);
 
  while (1){ 
    pass++;
  	best_cut_old = best_cut_new;
    for (i = 1; i<=counter; i++){
        base_cell = cell(bktA, bktB, noc, graph, max_node_id, &maxgainA, &maxgainB, &sizeA, &sizeB);
        gain = noc[base_cell].gains;
        //cut = cut - noc[base_cell].gains;
             
        if (noc[base_cell].BLK == 0)
           bkt_update (base_cell, noc[base_cell].gains, noc[base_cell].gains,1, bktA, bktB, p_max, noc, &maxgainA, &maxgainB);
         else
           bkt_update (base_cell, noc[base_cell].gains, noc[base_cell].gains,1, bktA, bktB, p_max, noc, &maxgainA, &maxgainB);
        gain_update (base_cell, graph,noc, con, bktA, bktB, p_max, &cut, &maxgainA, &maxgainB); //gain_update calls bkt_update
        initialize_cutset (con, max_node_id, &cut);
        insert_fclist (&free_cells, base_cell, &cut);
        if (best_cut_new > cut)
            best_cut_new  = cut;
        flag = 0;
   }
     if (best_cut_old <= best_cut_new){
        flag = 1;
      }
    if (flag == 0)
       reset (free_cells, con, noc, &best_cut_new, graph,max_node_id, &sizeA, &sizeB);
     else
       reset_all (free_cells, noc, &sizeA, &sizeB);
    update_net_array (graph, con, noc, max_node_id);
    gain_calculator (graph, noc, max_node_id,  con);
    bucket_list (graph, noc, max_node_id, bktA, bktB, p_max, &maxgainA, &maxgainB);
    if (flag ==1)
        break;
     
   }
  
  printf("Best cut = %d, Pass = %d\n", best_cut_new, pass);
	end = clock();
	double duration = ((double)end - start)/CLOCKS_PER_SEC;
  printf("Time : %f\n", duration);
 /* ************************************PRINT FUNCTIONS**********************************
    print_noc(graph, noc, max_node_id);
    prinf_fclist(free_cell);
    print_con(graph, con, max_node_id);
    print_bucket_list (bktA, bktB, p_max );

*****************************************************************************************/
}

























  
