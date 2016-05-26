#include "../watcher/watcher.h"
#include "../node/node.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

bool debugging=true;

uint8_t width=0;
uint8_t height=0;
uint8_t limit=0;

bool* active_rows;
bool* active_cols;
uint8_t** goal;


Node* read_input(char* filename)
{
   Node* r = malloc(sizeof(Node));
   FILE* file = fopen(filename, "r");

   char buf[256];
   fscanf(file,"%s", buf);
   fscanf(file,"%hhu", &height);
   fscanf(file,"%s", buf);
   fscanf(file,"%hhu", &width);


   active_rows = malloc(sizeof(bool) * height);
   for(uint8_t row = 0; row < height; row++)
   {
     active_rows[row] = false;
   }
   active_cols = malloc(sizeof(bool) * width);
   for(uint8_t col = 0; col < height; col++)
   {
     active_cols[col] = false;
   }

   uint8_t actives;
   uint8_t active;
   fscanf(file,"%hhu", &actives);
   for(uint8_t col = 0; col < actives; col++)
   {
     fscanf(file,"%hhu", &active);
     active_cols[active] = true;
   }
   fscanf(file,"%hhu", &actives);
   for(uint8_t row = 0; row < actives; row++)
   {
     fscanf(file,"%hhu", &active);
     active_rows[active] = true;
   }

   r->state = malloc(sizeof(uint8_t*) * height);
   for(uint8_t row = 0; row < height; row++)
   {
     r->state[row] = malloc(sizeof(uint8_t) * width);
     for(uint8_t col = 0; col < width; col++)
     {
       fscanf(file,"%hhu", &r->state[row][col]);
     }
   }

   fscanf(file,"%s", buf);
   fscanf(file,"%hhu", &limit);

   goal = malloc(sizeof(uint8_t*) * height);
   for(uint8_t row = 0; row < height; row++)
   {
     goal[row] = malloc(sizeof(uint8_t) * width);
     for(uint8_t col = 0; col < width; col++)
     {
       fscanf(file,"%hhu", &goal[row][col]);
     }
   }

   fclose(file);
   return r;

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

void print_global_variables()
{
  printf("\n-------------------------------------\n");
  printf("Global variables:\n");
  printf("Height: %hhu \n", height );
  printf("Width: %hhu \n", width );
  printf("Limit: %hhu \n", limit );

  printf("Active columns: ");
  for(uint8_t col = 0; col < width; col++)
  {
    if (active_cols[col])
    {
      printf("%hhu ",col );
    }
  }
  printf("\n");

  printf("Active rows:    ");
  for(uint8_t row = 0; row < height; row++)
  {
    if (active_rows[row])
    {
      printf("%hhu ",row );
    }
  }
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

int main(int argc, char *argv[])
{

  Node* root= read_input(argv[1]);

  if (debugging)
  {
    print_global_variables();
    print_node(root);
  }

  watcher_open(argv[1]);

  //TODO SOLVE 

  watcher_close();

  //TODO destroy tree
  //TODO destroy global variables

  return 0;
}
