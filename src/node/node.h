#include <stdint.h>
#include <stdbool.h>
//#include <gmp.h>

struct node;

typedef struct node Node;

struct node
{
  //The puzzle itself
  uint8_t** state;

  //WARNING There might be more childs than this
  int child_count;

  //The array of children nodes
  Node** childs;

  //Reference to the parent node
  Node* parent;

  //Direction of the movement that lead us to this state (R,L,U,D)
  char direction;

  //Number of the row or column that the previous shift was performed on
  int n;

  //mpz_t key;

};
