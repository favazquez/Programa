#include <stdint.h>

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

};

struct checked_before;

typedef struct checked_before Checked_Before;

struct checked_before
{
  //The puzzle itself
  uint8_t** state;
};
