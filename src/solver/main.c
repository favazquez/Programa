#include "../watcher/watcher.h"
#include "../node/node.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

bool debugging=true;
bool watch=true;

uint8_t width=0;
uint8_t height=0;
uint8_t limit=0;
uint8_t active_col_count=0;
uint8_t active_row_count=0;

bool* active_rows;
bool* active_cols;

uint8_t** goal;

Node* init_root()
{
  Node* r = malloc(sizeof(Node));

  r->state = malloc(sizeof(uint8_t*) * height);
  for(uint8_t row = 0; row < height; row++)
    r->state[row] = malloc(sizeof(uint8_t) * width);

  r->child_count=0;

  return r;
}

Node* init_node(Node* parent)
{
  Node* n = malloc(sizeof(Node));

  n->state = malloc(sizeof(uint8_t*) * height);
  for(uint8_t row = 0; row < height; row++)
    n->state[row] = malloc(sizeof(uint8_t) * width);

  for(uint8_t row = 0; row < height; row++)
    for(uint8_t col = 0; col < width; col++)
      n->state[row][col]=parent->state[row][col];

  n->child_count=0;

  return n;
}
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

Node* read_input(char* filename)
{
   FILE* file = fopen(filename, "r");

   char buf[256];

   fscanf(file,"%s", buf);
   fscanf(file,"%hhu", &height);
   fscanf(file,"%s", buf);
   fscanf(file,"%hhu", &width);

   active_rows = malloc(sizeof(bool) * height);

   for(uint8_t row = 0; row < height; row++)
     active_rows[row] = false;

   active_cols = malloc(sizeof(bool) * width);

   for(uint8_t col = 0; col < height; col++)
     active_cols[col] = false;

   uint8_t active;

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

   Node* r = init_root();

   for(uint8_t row = 0; row < height; row++)
     for(uint8_t col = 0; col < width; col++)
       fscanf(file,"%hhu", &r->state[row][col]);

   fscanf(file,"%s", buf);
   fscanf(file,"%hhu", &limit);

   goal = malloc(sizeof(uint8_t*) * height);

   for(uint8_t row = 0; row < height; row++)
     goal[row] = malloc(sizeof(uint8_t) * width);

   for(uint8_t row = 0; row < height; row++)
     for(uint8_t col = 0; col < width; col++)
       fscanf(file,"%hhu", &goal[row][col]);


   fclose(file);
   return r;

}



void print_global_variables()
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

void gen_children(Node* n)
{
  //This are ALL posible children, including dupes and maybe even the parent state
  n->child_count = 2*(active_col_count+active_row_count);
  n->childs=malloc(sizeof(Node)*n->child_count);

  for (int8_t i = 0; i < n->child_count; i++)
    n->childs[i]=init_node(n);

  int8_t idx=0;

  for(uint8_t row = 0; row < height; row++)
  {
    if (active_rows[row])
    {
      //shift right
      for (size_t j = 1; j < width; j++)
        n->childs[idx]->state[row][j]=n->state[row][j-1];

      n->childs[idx]->state[row][0]=n->state[row][width-1];

      print_node(n->childs[idx]);
      idx++;
      //shift left NOT WORKING PROPERLY
      for (size_t j = 0; j < width-2; j++)
        n->childs[idx]->state[row][j]=n->state[row][j+1];

      n->childs[idx]->state[row][width-1]= n->state[row][0];

      print_node(n->childs[idx]);
      idx++;


    }

  }

  for(uint8_t col = 0; col < height; col++)
  {
    if (active_cols[col])
    {
      //shift up
      //shift down
    }
  }


}
int main(int argc, char *argv[])
{

  if (watch)
    watcher_open(argv[1]);

  Node* root= read_input(argv[1]);

  if (debugging)
  {
    print_global_variables();
    printf("Root:");
    print_node(root);
    printf("-----------------------------------\n\n");
  }

  gen_children(root);


  //TODO SOLVE
  if (watch)
  {
    while (2>1)
    {
      /* code */
    }

    watcher_close();
  }





  //TODO destroy tree
  //TODO destroy global variables

  return 0;
}
