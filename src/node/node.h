#include <stdint.h>

struct node;

typedef struct node Node;

struct node
{
  uint8_t** state;

  //WARNING There might be more childs than this
  int child_count;
  Node** childs;

};
