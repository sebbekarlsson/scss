#include "include/test_utils.h"
#include <stdio.h>

#define AST scss_AST_T

void test_basic() {
  AST *root = run_get_ast("sources/basic.scss");
  ASSERT(root != 0, "root exists.");
  printf("%d\n", root->type);
  ASSERT(root->type == AST_COMPOUND, "root is compound");
  ASSERT(root->list_value != 0, "compound has list");
  ASSERT(root->list_value->size == 1, "list.size == 1");

  AST *child = (AST *)root->list_value->items[0];

  ASSERT(child != 0, "child is not null.");
  ASSERT(child->type == AST_STYLE_RULE, "child.type == AST_STYLE_RULE");
  ASSERT(child->body != 0, "child.body != 0");
  ASSERT(child->body->list_value != 0, "child.body.list_value != 0");
  ASSERT(child->body->list_value->size == 4, "child body contains 4 elements");

  for (unsigned int i = 0; i < child->body->list_value->size; i++) {
    AST *el = (AST *)child->body->list_value->items[i];

    ASSERT(el != 0, "element in child is not null.");
    ASSERT(el->type == AST_PROP_DEC, "element is of type AST_PROP_DEC");
  }
}

int main(int argc, char *argv[]) {
  test_basic();
  return 0;
}
