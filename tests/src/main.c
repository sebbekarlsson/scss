#include "include/test_utils.h"
#include <stdio.h>

#define AST scss_AST_T

void test_basic()
{
  AST* root = run_get_ast("sources/basic.scss");
  ASSERT(root != 0, "root exists.");
  ASSERT(root->type == SCSS_AST_COMPOUND, "root is compound");
  ASSERT(root->list_value != 0, "compound has list");
  ASSERT(root->list_value->size == 1, "list.size == 1");

  AST* child = (AST*)root->list_value->items[0];

  ASSERT(child != 0, "child is not null.");
  ASSERT(child->type == SCSS_AST_STYLE_RULE, "child.type == SCSS_AST_STYLE_RULE");
  ASSERT(child->body != 0, "child.body != 0");
  ASSERT(child->body->list_value != 0, "child.body.list_value != 0");
  ASSERT(child->body->list_value->size == 4, "child body contains 4 elements");

  for (unsigned int i = 0; i < child->body->list_value->size; i++) {
    AST* el = (AST*)child->body->list_value->items[i];

    ASSERT(el != 0, "element in child is not null.");
    ASSERT(el->type == SCSS_AST_PROP_DEC, "element is of type SCSS_AST_PROP_DEC");
  }
}

void test_nested()
{
  AST* root = run_get_ast("sources/nested.scss");
  ASSERT(root != 0, "root exists.");
  ASSERT(root->type == SCSS_AST_COMPOUND, "root is compound");
  ASSERT(root->list_value != 0, "compound has list");
  ASSERT(root->list_value->size == 1, "list.size == 1");

  AST* child = (AST*)root->list_value->items[0];
  ASSERT(child != 0, "child is not null.");
  ASSERT(child->type == SCSS_AST_STYLE_RULE, "child.type == SCSS_AST_STYLE_RULE");
  ASSERT(child->siblings != 0, "child.footer is not null.");
  ASSERT(child->siblings->size > 0, "child.footer size > 0");

  for (unsigned int i = 0; i < child->siblings->size; i++) {
    AST* el = (AST*)child->siblings->items[i];

    ASSERT(el != 0, "element in child footer is not null.");
    ASSERT(el->type == SCSS_AST_STYLE_RULE, "element is of type SCSS_AST_STYLE_RULE_DEC");
  }
}

int main(int argc, char* argv[])
{
  test_basic();
  test_nested();
  return 0;
}
