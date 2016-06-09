#include "../watcher/watcher.h"
#include "../node/node.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

// Enables prints associated to the debugging
bool debugging=false;

//Enables the prints associated to the initial state
bool print_init=false;

//Enables the watcher
bool watch=false;

//Puzzle-specific parameters
uint8_t width=0;
uint8_t height=0;
uint8_t limit=0;
uint8_t active_col_count=0;
uint8_t active_row_count=0;
bool* active_rows;
bool* active_cols;


uint8_t** goal;

//Node** visited;

//count of different states per level
/*count_level_states= count_nodes_nivel_anterior*2*(active_col_count+active_row_count) + count_level_states_previous*/
//int count_level_states=1;
//int count_level_states_previous=0;
//int index_states=0;
//int nodes_previous_level=0;
//uint8_t*** checked_before;


//Returns true if the nodes represent the same state
bool compare_nodes(Node* a, Node* b)
{
  for(uint8_t row = 0; row < height; row++)
    for(uint8_t col = 0; col < width; col++)
      if (a->state[row][col]!=b->state[row][col])
        return false;

  return true;
}

//Free used memory
void tree_destroy(Node* r)
{

  if (r->child_count<1)
  {
    for (size_t row = 0; row < height; row++)
     {
        free(r->state[row]);
    }
    free(r->state);
    //free(r->childs);
    free(r);
  }
  else
  {
    for (size_t i = 0; i < r->child_count; i++)
      {
        tree_destroy(r->childs[i]);
        //free(r->childs[i]->childs);
      }
      for (size_t row = 0; row < height; row++)
       {
          free(r->state[row]);
      }
    free(r->state);
    free(r->childs);
    free(r);
  }
}

/*unsigned long long mpz2ull(mpz_t z)
{
    unsigned long long result = 0;
    mpz_export(&result, 0, -1, sizeof result, 0, 0, z);
    return result;
}
void get_hash2(Node* n)
{
  int* hash;
  hash=malloc(sizeof(int)*8192);
  for (size_t i = 0; i < 8192; i++)
   hash[i]=0;

  int aux=9;
  int idx=0;
  int count=0;
  bool first=true;

  for(uint8_t row = 0; row < height; row++)
    for(uint8_t col = 0; col < width; col++)
      if ((active_rows[row])||(active_cols[col]))
      {
          if (first)
          {
            aux=n->state[row][col];
            count=1;
            first=false;
          }
          else
          {
            if (aux==n->state[row][col]&&count<9)
            {
              count++;
            }
            else
            {
                hash[idx]=count;
                idx++;
                hash[idx]=aux;
                idx++;

                aux=n->state[row][col];
                count=1;
            }
          }
      }

      for (size_t i = 0; i < idx; i++)
        printf("%i",hash[i] );


      printf("\n");
}
unsigned long long get_hash(Node* n)
{
  mpz_t hash;
  mpz_t resto;
  mpz_init(hash);
  mpz_init(resto);
  int idx=0;

  for(uint8_t row = 0; row < height; row++)
    for(uint8_t col = 0; col < width; col++)
      if ((active_rows[row])||(active_cols[col]))
      {
        mpz_add_ui(hash,hash,n->state[row][col]*pow(8,idx));
        idx++;
      }

  int l = INT32_MAX;
  mpz_mod_ui(resto,hash,l);
  l=mpz2ull(resto);
  return l;

}*/
void destroy_global_parameters()
{//nose si va esto aca... segun yo solo es necesario si lo pides con malloc(no es el caso), ya que el resto vive solo en el scope
  free(active_cols);
  free(active_rows);
  for (size_t i = 0; i < height; i++)
    free(goal[i]);
  free(goal);
}

//Updates the watcher
void update_watcher(Node* s)
{
  sleep(1);

  if ((s->direction=='R')||(s->direction=='L'))
    for (size_t i = 0; i < width; i++)
      watcher_update_cell(s->n, i, s->state[s->n][i]);

  if ((s->direction=='U')||(s->direction=='D'))
    for (size_t i = 0; i < height; i++)
      watcher_update_cell(i, s->n, s->state[i][s->n]);
}

//Prints the solution
void print_solution(Node* s)
{
  if(s->direction=='X')
    return;
  else
  {
    print_solution(s->parent);
    printf("%c %i\n", s->direction,s->n);
    if (watch)
      update_watcher(s);
  }
}

//Initializes and returns the root
Node* init_root()
{
  //Allocate memory
  Node* r = malloc(sizeof(Node));
  r->state = malloc(sizeof(uint8_t*) * height);
  for(uint8_t row = 0; row < height; row++)
    r->state[row] = malloc(sizeof(uint8_t) * width);

  //set child count to 0 ( this the sole condition that specifies the node is a leaf)
  r->child_count=0;

  r->direction='X';
  r->n=999;

  return r;
}

//Initializes a node. Receives the parent node and returns the child (empty)
Node* init_node(Node* parent)
{
  //Memory allocation
  Node* n = malloc(sizeof(Node));
  n->parent=parent;
  n->state = malloc(sizeof(uint8_t*) * height);
  for(uint8_t row = 0; row < height; row++)
    n->state[row] = malloc(sizeof(uint8_t) * width);

  //Copies the state of the parent node
  //NOTE: THIS IS INEFFICIENT AF
  for(uint8_t row = 0; row < height; row++)
    for(uint8_t col = 0; col < width; col++)
      n->state[row][col]=parent->state[row][col];

  //set child count to 0 ( this the sole condition that specifies the node is a leaf)
  n->child_count=0;
  n->direction='M';
  n->n=6969;

  return n;
}
int* crear_tabla_auxiliar()
{
  int* aux = malloc(sizeof(int)*UINT16_MAX);
  return aux;
}

//Prints the info asssociated to the node
void print_node(Node* n)
{
  for(uint8_t row = 0; row < height; row++)
  {
    for(uint8_t col = 0; col <width; col++)
    {
      printf("%hhu ", n->state[row][col]);
    }
    printf("\n");
  }
  printf("The shift that led to this node was:\n");
  printf("%c %i\n", n->direction,n->n);

}
/*
void checked_before_init()
{
  //crea un array de tres dimensiones

  checked_before=malloc(sizeof(uint8_t)*count_level_states+sizeof(uint8_t)*index_states);

  for (size_t idx = 0; idx < count_level_states+count_level_states_previous; idx++)
  {
    checked_before[idx]=malloc(sizeof(uint8_t*) * height);
    for(uint8_t row = 0; row < height; row++)
      checked_before[idx][row] = malloc(sizeof(uint8_t) * width);
  }

}
void resize_checked_before()
{
  //its the minimum space we need to add to our matrix

  int child_count= nodes_previous_level*2*(height+width);
  checked_before= realloc(checked_before, (index_states+child_count)*sizeof(uint8_t));

}
void add_state_checked(uint8_t** state)
{
  //first we create space to save our state inside the 3D matrix
  checked_before[index_states]=malloc(sizeof(uint8_t*) * height);
  for(uint8_t row = 0; row < height; row++)
    checked_before[index_states][row] = malloc(sizeof(uint8_t) * width);
  //we save the state in the matrix and add 1 to the count of checked states
  checked_before[index_states++]=state;
  nodes_previous_level+=1;

}


//Checks if we return to a state we have checked before:
//case we haven't: adds it to our list and returns false
//case we have: returns true, so we can  destroy this node/child

bool node_checked_before(uint8_t** check_state)
{

  //iteration over our matrix states
  for (size_t i = 0; i<index_states; i++)
  {
    //bool gets false in case some space in the state matrix to be checked is different
    //to one space in one of the previous checked states
    bool checked=true;
    for(uint8_t row = 0; row < height; row++)
      for(uint8_t col = 0; col < width; col++)
        if (check_state[row][col]!=checked_before[i][row][col])
          checked= false;
    //if after one iteration the boolean is still true we return it, because it means that
     //we have checked this state before
    if(checked)
      return checked;

  }
  //if after iterating over the whole matrix we haven't returned anything we return false
  return false;
}
*/
//Prints the global parameters. Duh.
void print_global_parameters()
{
  printf("\n-------------------------------------\n");
  printf("Global variables:\n");
  printf("Height: %hhu \n", height );
  printf("Width: %hhu \n", width );
  printf("Limit: %hhu \n", limit );
  printf("There are %hhu active columns, which are : ",active_col_count);
  for(uint8_t col = 0; col < width; col++)
    if (active_cols[col])
      printf("%hhu ",col );
  printf("\n");
  printf("There are %hhu active rows, which are    : ",active_row_count);
  for(uint8_t row = 0; row < height; row++)
    if (active_rows[row])
      printf("%hhu ",row );
  printf("\n");
  printf("Goal State:\n");
  for(uint8_t row = 0; row < height; row++)
  {
    for(uint8_t col = 0; col < width; col++)
    {
      printf("%hhu ", goal[row][col]);
    }
    printf("\n");
  }
  printf("-------------------------------------\n");
}

//Reads the input file.
//Sets the global parameters
//Generates and returns the root
Node* read_input(char* filename)
{
    //Opens the file
   FILE* file = fopen(filename, "r");

   //Create buffers
   char buf[256];
   uint8_t active;

   //Scan the file
   fscanf(file,"%s", buf);
   fscanf(file,"%hhu", &height);
   fscanf(file,"%s", buf);
   fscanf(file,"%hhu", &width);

   //Allocate memory
   active_rows = malloc(sizeof(bool) * height);
   for(uint8_t row = 0; row < height; row++)
     active_rows[row] = false;
   active_cols = malloc(sizeof(bool) * width);
   for(uint8_t col = 0; col < height; col++)
     active_cols[col] = false;

    //Scan
   fscanf(file,"%hhu", &active_col_count);
   for(uint8_t col = 0; col < active_col_count; col++)
   {
     fscanf(file,"%hhu", &active);
     active_cols[active] = true;
   }
   fscanf(file,"%hhu", &active_row_count);
   for(uint8_t row = 0; row < active_row_count; row++)
   {
     fscanf(file,"%hhu", &active);
     active_rows[active] = true;
   }

   //Create empty root
   Node* r = init_root();
   //creates struct that will keep every state that was checked before
   //checked_before_init();
   //Scan
   for(uint8_t row = 0; row < height; row++)
     for(uint8_t col = 0; col < width; col++)
       fscanf(file,"%hhu", &r->state[row][col]);
   fscanf(file,"%s", buf);
   fscanf(file,"%hhu", &limit);

   //Allocate memory
   goal = malloc(sizeof(uint8_t*) * height);
   for(uint8_t row = 0; row < height; row++)
     goal[row] = malloc(sizeof(uint8_t) * width);


    //Scan
    for(uint8_t row = 0; row < height; row++)
    for(uint8_t col = 0; col < width; col++)
    {
      fscanf(file,"%hhu", &goal[row][col]);
    }
    //checked_before[0]=goal; //<- ojo que esto esta copiando por referencia

   //Close the file and return the root
   fclose(file);

   return r;

}


Node* shift_right(Node* n,size_t row)
{
  Node* p = init_node(n);
  //This loop shifts the row to the right except for the first element
  for (size_t j = 1; j < width; j++)
    n->state[row][j]=p->state[row][j-1];

  //Does the shift for the first element
  n->state[row][0]=p->state[row][width-1];

  //Saves the info associated with the shift
  n->direction='R';
  n->n=row;

  return n;
}

Node* shift_left(Node* n,size_t row)
{
  Node* p = init_node(n);
  //Shifts the row to the left except for the last element
  for (size_t j = 0; j < width-1; j++)
    n->state[row][j]=p->state[row][j+1];

  //Does the shift for the last element
  n->state[row][width-1]= p->state[row][0];

  //Saves the info associated with the shift
  n->direction='L';
  n->n=row;

  return n;

}

Node* shift_up(Node* n,size_t col)
{
  Node* p = init_node(n);
  //shifts the column up except for the last element
  for (size_t j = 0; j < height-1; j++)
    n->state[j][col]=p->state[j+1][col];

  //shifts up the last element
  n->state[height-1][col]=p->state[0][col];

  //Saves the info associated with the shift
  n->direction='U';
  n->n=col;

  return n;

}

Node* shift_down(Node* n,size_t col)
{
  Node* p = init_node(n);
  //shifts the column down except for the first element
  for (size_t j = 1; j < height; j++)
    n->state[j][col]=p->state[j-1][col];

  //shifts down the first element
  n->state[0][col]=p->state[height-1][col];

  //Saves the info associated with the shift
  n->direction='D';
  n->n=col;

  return n;

}
void print_children(Node* n)
{
  printf("--------------------------------------\nThe node :\n");
  print_node(n);
  printf("Has the following children:\n");
  for (size_t i = 0; i < n->child_count; i++)
    print_node(n->childs[i]);
}
//Generates the children of a given node, corresponding to all posible moves from the parent node
void gen_children(Node* n, size_t depth)
{

  //This are ALL posible children, including dupes and maybe even the parent state
  n->child_count = 2*(active_col_count+active_row_count);

  //Allocate memory
  n->childs=malloc(sizeof(Node)*n->child_count);

  //Create the appropiate number of empty nodes
  for (int8_t i = 0; i < n->child_count; i++)
    n->childs[i]=init_node(n);

  //Children index
  int8_t idx=0;


  //Loops through the active rows.
  for(uint8_t row = 0; row < height; row++)
  {
    if (active_rows[row])
    {
      n->childs[idx]= shift_right(n->childs[idx],row);

      if (!compare_nodes(n, n->childs[idx]))
      {
        idx++;
      }
      else
      {
        n->childs[idx]= shift_left(n->childs[idx],row);
      }


      n->childs[idx]= shift_left(n->childs[idx],row);

      if (!compare_nodes(n, n->childs[idx]))
      {
        idx++;
      }
      else
      {
        n->childs[idx]= shift_right(n->childs[idx],row);
      }

    }
  }

  //Loops through the active columns
  for(uint8_t col = 0; col < width; col++)
  {
    if (active_cols[col])
    {
      n->childs[idx]= shift_up(n->childs[idx],col);

      if (!compare_nodes(n, n->childs[idx]))
      {
        idx++;
      }
      else
      {
        n->childs[idx]=shift_down(n->childs[idx],col);
      }

      n->childs[idx]= shift_down(n->childs[idx],col);

      if (!compare_nodes(n, n->childs[idx]))
      {
        idx++;
      }
      else
      {
        n->childs[idx]=shift_up(n->childs[idx],col);
      }

    }
  }

//Free the memory of unused childs
  if(idx<n->child_count)
  {
    for (size_t i = idx+1; i < n->child_count; i++) {
      for (size_t row = 0; row < height; row++)
       {
          free(n->childs[i]->state[row]);
      }
      free(n->childs[i]->state);
      free(n->childs[i]);
    }
  }

  //Update to actual children count
  n->child_count=idx;

  if (debugging)
    print_children(n);
}

//Checks if the given node is the goal. Returns true in this case.
bool check_goal(Node* node)
{
  for(uint8_t row = 0; row < height; row++)
    for(uint8_t col = 0; col < width; col++)
      if (goal[row][col]!=node->state[row][col])
        return false;

  return true;
}

//Does a depth-limited DFS
Node* DLS(Node* node,size_t depth)
{
  if ((depth==0)&&(check_goal(node)))
    return node;
  else if (depth>0)
  {
    if (debugging)
      printf("Trying to generate the children nodes\n");

    if (node->child_count<1)
        gen_children(node,depth);


    for (size_t i = 0; i < node->child_count; i++)
    {
      if (debugging)
        printf("Searching child %zu at depth %zi\n",i,(depth-1));

      /*

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                          ACHTUNG!!!!!!!!!

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1


      */

      Node* solution = DLS(node->childs[i],depth-1);

      if (solution)
        return solution;
    }
  }
  if (debugging)
    printf("No solution found at depth %zu\n",depth);
  return NULL;
}

//Does iterative deepening depth-first search to find a solution
Node* IDDFS(Node* root)
{
  //this loop iteratively increases the depth
  //at which we try to find the solution
  for (size_t i = 1; i < limit+1; i++)
  {
    if (debugging)
      printf("Trying to find the solution at depth %zu\n",i );

    //reallocates our memory space used to save states checked before

    //resize_checked_before();

    //tries to find the solution at the given depth
    Node* solution = DLS(root,i);
    //set to 0 if we start a new level
    //nodes_previous_level=0;
    //if it found a solution it immediately returns it
    //NOTE this solution is in fact optimal
    if (solution)
      return solution;
  }

  //if it did not find the solution in the given limit it returns null
  return NULL;
}

int main(int argc, char *argv[])
{

  if (watch)
    watcher_open(argv[1]);

  //Generate root and global parameters from the file input
  Node* root= read_input(argv[1]);

  //Prints stuff
  if (print_init)
  {
    print_global_parameters();
    printf("Root:\n");
    print_node(root);
    printf("-----------------------------------\n\n");
  }

  //Clock
  clock_t start = clock();

  //Init the list of visited states
  /*
  visited=malloc(sizeof(Node)*INT32_MAX);
  visited[0]=root;
  if (visited[0])
    print_node(visited[0]);
    */
  //print_node(root);
  //get_hash2(root);

  //Tries to find the solution by using iterative deepening depth-first search
  Node* solution = IDDFS(root);

  double time_used = ((double) (clock() - start)) / CLOCKS_PER_SEC;

  fprintf(stderr, "Optimal solution found in %lf seconds\n", time_used);

  //Prints the solution if it found one. If it did not prints IMPOSSIBIRU
  if (solution)
  {
    printf("------------------------\n");
    printf("The optimal solution is:\n");
    print_solution(solution);
  }
  else
    printf("IMPOSSIBIRU\n");

  if (watch)
  {
    sleep(3);
    watcher_close();
  }

  tree_destroy(root);
  /*
  for (size_t i = 0; i < 100000; i++) {
    int* aux= crear_tabla_auxiliar();
    free(aux);
  }*/
  destroy_global_parameters();


  return 0;
}
