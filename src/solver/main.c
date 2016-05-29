#include "../watcher/watcher.h"
#include "../node/node.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

// Enables prints associated to the debugging
bool debugging=false;

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

//count of different states per level
/*count_level_states= count_nodes_nivel_anterior*2*(active_col_count+active_row_count) + count_level_states_previous*/
int count_level_states=1;
int count_level_states_previous=0;
int index_states=0;
int nodes_previous_level=0;
uint8_t*** checked_before;


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
    free(r); // segun yo tambien va un free(r->chields)
  else
  {
    for (size_t i = 0; i < r->child_count; i++)
      tree_destroy(r->childs[i]);

    free(r->childs);
    free(r);
  }
}

void destroy_global_parameters()
{//nose si va esto aca... segun yo solo es necesario si lo pides con malloc(no es el caso), ya que el resto vive solo en el scope
  free(active_cols);
  free(active_rows);
  for (size_t i = 0; i < height; i++)
    free(goal[i]);
  free(goal);
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

  return r;
}

//Initializes a node. Receives the parent node and returns the child (empty)
Node* init_node(Node* parent)
{
  //Memory allocation
  Node* n = malloc(sizeof(Node));
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

  return n;
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
  printf("\n");
}
void checked_before_init()
{
  //crea un array de tres dimensiones

  checked_before=malloc(sizeof(uint8_t)*count_level_states+sizeof(uint8_t)*index_states);

  /*for (size_t idx = 0; idx < count_level_states+count_level_states_previous; idx++)
  {
    checked_before[idx]=malloc(sizeof(uint8_t*) * height);
    for(uint8_t row = 0; row < height; row++)
      checked_before[idx][row] = malloc(sizeof(uint8_t) * width);
  }*/

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


/*Checks if we return to a state we have checked before:
case we haven't: adds it to our list and returns false
case we have: returns true, so we can  destroy this node/child*/
bool node_checked_before(uint8_t** check_state)
{

  //iteration over our matrix states
  for (size_t i = 0; i<index_states; i++)
  {
    /*bool gets false in case some space in the state matrix to be checked is different
    to one space in one of the previous checked states*/
    bool checked=true;
    for(uint8_t row = 0; row < height; row++)
      for(uint8_t col = 0; col < width; col++)
        if (check_state[row][col]!=checked_before[i][row][col])
          checked= false;
    /*if after one iteration the boolean is still true we return it, because it means that
     we have checked this state before*/
    if(checked)
      return checked;

  }
  //if after iterating over the whole matrix we haven't returned anything we return false
  return false;
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
   checked_before_init();
   printf("%s\n","post");
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
    checked_before[0]=goal;
   //Close the file and return the root
   fclose(file);

   return r;

}

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

//Generates the children of a given node, corresponding to all posible moves from the parent node
void gen_children(Node* n)
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
      //When the row is active we need to generate two children for it:
      //One corresponding to shift left and another for shift right

      //First we generate the children node associated to  shift right

      //This loop shifts the row to the right except for the first element
      for (size_t j = 1; j < width; j++)
        n->childs[idx]->state[row][j]=n->state[row][j-1];

      //Does the shift for the first element
      n->childs[idx]->state[row][0]=n->state[row][width-1];

      //Prints the resulting children node (only when debugging)
      if (debugging)
      {
        printf("Result of shift right on row %hhu\n",row);
        print_node(n->childs[idx]);
      }


      //Increase the index so a different children is modified
      idx++;

      //Now we generate the children node associated to the shift left
      //Es posible juntar ambos en un solo for(más eficiente)

      //Shifts the row to the left except for the last element
      for (size_t j = 0; j < width-1; j++)
        n->childs[idx]->state[row][j]=n->state[row][j+1];

      //Does the shift for the last element
      n->childs[idx]->state[row][width-1]= n->state[row][0];

      //Prints the resulting children node (only when debugging)
      if (debugging)
      {
        printf("Result of shift left on row %hhu\n",row);
        print_node(n->childs[idx]);
      }

      //Increase the index so a different children is modified
      idx++;
    }
  }

  //Loops through the active columns
  for(uint8_t col = 0; col < width; col++)
  {
    if (active_cols[col])
    {
      //shifts the column up except for the last element
      for (size_t j = 0; j < height-1; j++)
        n->childs[idx]->state[j][col]=n->state[j+1][col];

      //shifts up the last element
      n->childs[idx]->state[height-1][col]=n->state[0][col];

      //Prints the resulting children node (only when debugging)
      if (debugging)
      {
        printf("Result of shift up on col %hhu\n",col);
        print_node(n->childs[idx]);
      }

      //Increase the index so a different children is modified
      idx++;

      //shifts the column down except for the first element
      for (size_t j = 1; j < height; j++)
        n->childs[idx]->state[j][col]=n->state[j-1][col];

      //shifts down the first element
      n->childs[idx]->state[0][col]=n->state[height-1][col];

      //Prints the resulting children node (only when debugging)
      if (debugging)
      {
        printf("Result of shift down on col %hhu\n",col);
        print_node(n->childs[idx]);
      }

      //Increase the index so a different children is modified
      idx++;
    }
  }
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
      printf("This node is not a solution. Trying the children nodes\n");

    if (node->child_count<1)
      gen_children(node);

    for (size_t i = 0; i < node->child_count; i++)
    {
      if (debugging)
        printf("Searching child %zu at depth %zi\n",i,(depth-1));

      /*

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                          ACHTUNG!!!!!!!!!

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1

AQUI VA LA WEA QUE DEBERIAS HACER. LA IDEA SERIA QUE ANTES DE HACER LA DLS , VERIFIQUEMOS SI ESE ESTADO PARTICULAR PARA ESA PROFUNDIDAD PARTICULAR YA LO CONSULTAMOS

COMO HACERLO EFICIENTEMENTE? NO SE. SEGUN YO ESTA WEA VA A EMPEORAR LOS PUZZLES EASY Y COMO NO TENEMOS LOS NORMALS NO PODEMOS PROBAR SI ESTA SCHEISSE FUNCIONA BIEN

      */

      Node* solution = DLS(node->childs[i],depth-1);
      //como funciona esto??? Es un nodo, no boolean. Retorna false si es null?
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

    resize_checked_before();

    //tries to find the solution at the given depth
    Node* solution = DLS(root,i);
    //set to 0 if we start a new level
    nodes_previous_level=0;
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
  if (debugging)
  {
    print_global_parameters();
    printf("Root:\n");
    print_node(root);
    printf("-----------------------------------\n\n");
  }

  //Tries to find the solution by using iterative deepening depth-first search
  Node* solution = IDDFS(root);

  //Prints the solution if it found one. If it did not prints IMPOSSIBIRU
  if (solution)
  {
    printf("\n------------------------\n");
    printf("The solution is:\n");
    print_node(solution);
  }
  else
    printf("IMPOSSIBRU\n");

  //Prevents the watcher from crashing. why? because fuck u that's why.
  if (watch)
  {
    while (2>1)
    {

    }

    watcher_close();
  }


  tree_destroy(root);

  destroy_global_parameters();

  return 0;
}
