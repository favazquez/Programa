#include "../watcher/watcher.h"
#include "../node/node.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

// Enables prints associated to the debugging
bool debugging=true;

//Enables the watcher
bool watch=true;

//Puzzle-specific parameters
uint8_t width=0;
uint8_t height=0;
uint8_t limit=0;
uint8_t active_col_count=0;
uint8_t active_row_count=0;
bool* active_rows;
bool* active_cols;
uint8_t** goal;

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
  printf("\nNode State\n");
  for(uint8_t row = 0; row < height; row++)
  {
    for(uint8_t col = 0; col <width; col++)
    {
      printf("%hhu ", n->state[row][col]);
    }
    printf("\n");
  }
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
       fscanf(file,"%hhu", &goal[row][col]);

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
//NOTE: INCOMPLETE !! ALSO SHIFT LEFT DOESN'T WORK
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

  //Loops thorugh the active rows.
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
        print_node(n->childs[idx]);

      //Increase the index so a different children is modified
      idx++;

      //Now we generate the children node associated to the shift left

      //Shifts the row to the left except for the last element
      for (size_t j = 0; j < width-2; j++)
        n->childs[idx]->state[row][j]=n->state[row][j+1];

      //Does the shift for the last element
      //THIS IS NOT WORKING!!
      //TODO: FIX THIS
      n->childs[idx]->state[row][width-1]= n->state[row][0];

      //Prints the resulting children node (only when debugging)
      if (debugging)
        print_node(n->childs[idx]);

      //Increase the index so a different children is modified
      idx++;
    }
  }

  //Loops through the active columns
  for(uint8_t col = 0; col < height; col++)
  {
    if (active_cols[col])
    {
      //TODO shift up

      //Increase the index so a different children is modified
      idx++;

      //TODO shift down

      //Increase the index so a different children is modified
      idx++;
    }
  }
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
    printf("Root:");
    print_node(root);
    printf("-----------------------------------\n\n");
  }

  //Testing the function! This does not go here
  //gen_children(root);

  //TODO SOLVE

  //Prevents the watcher from crashing. why? because fuck u that's why.
  if (watch)
  {
    while (2>1)
    {

    }

    watcher_close();
  }


  //TODO destroy tree

  //TODO destroy global variables

  return 0;
}
