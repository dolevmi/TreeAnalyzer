#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

#define FAIL 0
#define SUCCESS 1
#define ARGS_NUM 4
#define FIRST_ARG 1
#define SECOND_ARG 2
#define THIRD_ARG 3
#define BASE 10
#define MAX_SENTENCE_LENGTH 1024
#define ASCII_LOWER_BOUND 48
#define ASCII_UPPER_BOUND 57

#define READ_ONLY "r"
#define DELIMITER " \n\r\t"
#define ENTER "\n"

#define ARGS_ERROR_MSG "Usage: TreeAnalyzer <Graph File Path>" \
" <First Vertex> <Second Vertex>\n"
#define INPUT_ERROR_MSG "Invalid input\n"
#define ALLOC_ERROR_MSG "Allocation Error\n"

typedef struct Node {
    int key;  //1
    int dist;
    struct Node *father;
    struct Node **children;
    int children_counter;
    int prev;
} Node;

/**
 * check if string is numeric
 * @param param
 * @return 1 upon success, 0 upon failure
 */
int is_numeric (char *param)
{
  for (long unsigned int i = 0; i < strlen (param); ++i)
    {
      //compare to ascii
      if ((int) param[i] < ASCII_LOWER_BOUND
          || ((int) param[i] > ASCII_UPPER_BOUND))
        {
          return FAIL;
        }
    }
  return SUCCESS;
}

/**
 * initialize all vertices
 * @param new_tree_ptr
 * @param vertices_num
 * @return 1 upon success, 0 upon failure
 */
int tree_initialization (Node **new_tree_ptr, int vertices_num)
{
  for (int i = 0; i < vertices_num; ++i)
    {
      new_tree_ptr[i] = malloc (sizeof (Node));
      if (!new_tree_ptr[i])
        {
          return FAIL;
        }
      new_tree_ptr[i]->key = i;
      new_tree_ptr[i]->dist = 0;
      new_tree_ptr[i]->father = NULL;
      new_tree_ptr[i]->children = NULL;
      new_tree_ptr[i]->children_counter = 0;
      new_tree_ptr[i]->prev = -1;
    }
  return SUCCESS;
}

/**
 * adding node to the tree
 * @param new_tree
 * @param cur_line
 * @param key
 * @param children_counter
 * @return 1 upon success, 0 upon failure
 */
int add_node (Node **new_tree, int cur_line, int key, int children_counter)
{
  new_tree[cur_line]->children = realloc (new_tree[cur_line]->children,
                                          sizeof (Node*) * children_counter);
  if (!new_tree[cur_line]->children)
    {
      fprintf (stderr, ALLOC_ERROR_MSG);
      return FAIL; //FAIL
    }
  if (new_tree[key]->father)
    {
      //circle detected (root is not included)
      fprintf (stderr, INPUT_ERROR_MSG);
      return FAIL;
    }
  new_tree[cur_line]->children[children_counter - 1] = new_tree[key];
  new_tree[key]->father = new_tree[cur_line];
  new_tree[cur_line]->children_counter = children_counter;
  return SUCCESS;
}

/**
 * checks if a graph is a tree
 * @param new_tree
 * @param vertices_num
 * @return 1 upon success, 0 upon failure
 */
int is_tree(Node** new_tree, int vertices_num)
{
  int no_father_counter = 0;
  int edges_num=0;

  for (int i = 0; i < vertices_num; ++i)
    {
      if (!new_tree[i]->father)
        {
          ++no_father_counter;
        }
      if (no_father_counter > 1){
          return FAIL;
        }
      edges_num += new_tree[i]->children_counter;
    }
  if (edges_num != vertices_num - 1)
    {
      return FAIL;
    }

  return SUCCESS;
}

/**
 * check if args are within the graph
 * @param vertex_val
 * @param first
 * @param second
 * @param legit_ptr1
 * @param legit_ptr2
 */
void check_vertex_args(int vertex_val, int first,
                       int second, int* legit_ptr1,
                       int* legit_ptr2)
{
  if (vertex_val == first)
    {
      *legit_ptr1 = 1;
    }
  if (vertex_val == second)
    {
      *legit_ptr2 = 1;
    }
}

/**
 * finding the root vertex
 * @param new_tree
 * @param vertices_num
 * @return Node* root
 */
Node* find_root(Node** new_tree, int vertices_num)
{
  Node* root = NULL;
  for (int i = 0; i < vertices_num; ++i)
    {
      if(!new_tree[i]->father)
        {
          root = new_tree[i];
        }
    }
  return root;
}

/**
 * bfs between 2 vertices
 * @param new_tree
 * @param start
 * @param end
 * @param vertices_num
 */
void bfs_2(Node** new_tree, Node** start, Node** end, int vertices_num)
{
  for (int i = 0; i < vertices_num; ++i)
    {
      new_tree[i]->dist = -1;
      new_tree[i]->prev = -1;
    }
  (*start)->dist = 0;
  Queue* new_queue = allocQueue();
  enqueue(new_queue, (*start)->key);
  while (!queueIsEmpty(new_queue))
    {
      unsigned int u_key = dequeue(new_queue);
      for (int j = 0; j < new_tree[u_key]->children_counter; ++j)
        {
          if (new_tree[u_key]->children[j]->dist==-1)
            {
              enqueue(new_queue, new_tree[u_key]->children[j]->key);
              new_tree[u_key]->children[j]->prev = (int) u_key;
              new_tree[u_key]->children[j]->dist = new_tree[u_key]->dist + 1;
            }
        }
      if (new_tree[u_key]->father && new_tree[u_key]->father->dist==-1)
        {
          enqueue(new_queue, new_tree[u_key]->father->key);
          new_tree[u_key]->father->prev = (int) u_key;
          new_tree[u_key]->father->dist = new_tree[u_key]->dist + 1;
        }
      if((*end)->key == (int) u_key)
        {
          freeQueue(&new_queue);
          return;
        }
    }
}

/**
 * standard bfs
 * @param new_tree
 * @param root
 * @param vertices_num
 */
void bfs(Node** new_tree, Node** root, int vertices_num)
{
  for (int i = 0; i < vertices_num; ++i)
    {
      new_tree[i]->dist = -1;
    }
  (*root)->dist = 0;
  Queue* new_queue = allocQueue();
  enqueue(new_queue, (*root)->key);
  while (!queueIsEmpty(new_queue))
    {
      unsigned int u_key = dequeue(new_queue);
      for (int j = 0; j < new_tree[u_key]->children_counter; ++j)
        {
          if (new_tree[u_key]->children[j]->dist==-1)
            {
              enqueue(new_queue, new_tree[u_key]->children[j]->key);
              new_tree[u_key]->children[j]->father = new_tree[u_key];
              new_tree[u_key]->children[j]->dist = new_tree[u_key]->dist + 1;
            }
        }
      if (new_tree[u_key]->father && new_tree[u_key]->father->dist==-1)
        {
          enqueue(new_queue, new_tree[u_key]->father->key);
          new_tree[u_key]->father->dist = new_tree[u_key]->dist + 1;
        }
    }
  freeQueue(&new_queue);
}

/**
 * minimal path
 * @param new_tree
 * @param vertices_num
 * @return int minimal distance
 */
int calc_min_path(Node** new_tree, int vertices_num)
{
  int min_dist = vertices_num;
  for (int i = 0; i < vertices_num; ++i)
    {
      //if node is a leaf + minimal dist from root
      if (!new_tree[i]->children && new_tree[i]->dist<min_dist)
        {
          min_dist = new_tree[i]->dist;
        }
    }
  return min_dist;
}

/**
 * maximal path
 * @param new_tree
 * @param vertices_num
 * @return int maximal distance
 */
int calc_max_path(Node** new_tree, int vertices_num)
{
  int max_dist = 0;
  for (int i = 0; i < vertices_num; ++i)
    {
      //if node is a leaf + maximal dist from root
      if (!new_tree[i]->children && new_tree[i]->dist>max_dist)
        {
          max_dist = new_tree[i]->dist;
        }
    }
  return max_dist;
}

/**
 * graph's diameter
 * @param new_tree
 * @param vertices_num
 * @param max_path
 * @return int diameter
 */
int calc_diameter(Node** new_tree, int vertices_num, int max_path)
{
  int diameter=max_path;
  for (int i = 0; i < vertices_num; ++i)
    {
      bfs(new_tree, &new_tree[i], vertices_num);
      int cur_diam = calc_max_path(new_tree, vertices_num);
      if (cur_diam > diameter)
        {
          diameter = cur_diam;
        }
    }
  return diameter;
}

/**
 * shortest path using bfs
 * @param first
 * @param second
 * @param new_tree
 * @param vertices_num
 * @return shortest path as int array
 */
int* calc_shortest_path(int first, int second,
                        Node** new_tree, int vertices_num)
{
  bfs_2(new_tree, &new_tree[first], &new_tree[second], vertices_num);

  int* shortest_path=calloc((new_tree[second]->dist+1), sizeof(int));
  if (!shortest_path)
    {
      return NULL;
    }

  Node* temp = new_tree[second];
  int i = new_tree[second]->dist;
  shortest_path[i] = temp->key;
  --i;
  while(i >= 0)
    {
      shortest_path[i]=temp->prev;
      --i;
      temp = new_tree[temp->prev];
    }
  return shortest_path;
}

/**
 * frees a tree
 * @param new_tree
 * @param vertices_num
 */
void free_tree(Node** new_tree, int vertices_num)
{
  for (int i = 0; i < vertices_num; ++i)
    {
      if(new_tree[i]->children)
        {
          free (new_tree[i]->children);
          new_tree[i]->children = NULL;
        }
      free(new_tree[i]);
      new_tree[i] = NULL;
    }
  free(new_tree);
  new_tree = NULL;
}

/**
 * main function for tree building
 * @param dir
 * @param first
 * @param second
 * @return 1 upon success, 0 upon failure
 */
int build_tree (char *dir, int first, int second)
{
  char *vertex;
  int vertex_val;
  int vertices_num;
  int lines = 1;
  int counter = 0;
  char line[MAX_SENTENCE_LENGTH];
  int legit_vertex1=0;
  int* legit_ptr1 = &legit_vertex1;
  int legit_vertex2=0;
  int* legit_ptr2 = &legit_vertex2;

  //check if file is legal
  FILE *f = fopen (dir, READ_ONLY);
  if (!f)
    {
      fprintf (stderr, INPUT_ERROR_MSG);
      return FAIL;
    }

  //if file is empty
  if (!fgets (line, MAX_SENTENCE_LENGTH, f))
    {
      fprintf (stderr, INPUT_ERROR_MSG);
      fclose (f);
      return FAIL;
    }

  vertex = strtok (line, DELIMITER);
  if (!is_numeric (vertex))
    {
      fprintf (stderr, INPUT_ERROR_MSG);
      fclose (f);
      return FAIL;
    }
  vertices_num = (int) strtol (vertex, NULL, BASE);
  if (vertices_num <= 0)
    {
      fprintf (stderr, INPUT_ERROR_MSG);
      fclose (f);
      return FAIL;
    }
  vertex = strtok (NULL, DELIMITER);
  if (vertex)
    {
      fprintf (stderr, INPUT_ERROR_MSG);
      fclose (f);
      return FAIL;
    }

  Node **new_tree = malloc (vertices_num * sizeof (Node *));
  if (!new_tree)
    {
      fprintf (stderr, ALLOC_ERROR_MSG);
      free_tree(new_tree,vertices_num);
      fclose (f);
      return FAIL;
    }

  if(!tree_initialization (new_tree, vertices_num))
    {
      fprintf (stderr, ALLOC_ERROR_MSG);
      free_tree(new_tree,vertices_num);
      fclose (f);
      return FAIL;
    }

  while (fgets (line, MAX_SENTENCE_LENGTH, f))
    {
      int children_counter = 0;
      ++lines;
      vertex = strtok (line, DELIMITER);
      while (vertex!=NULL && strcmp (vertex, "-") != 0)
        {
          if (!is_numeric (vertex) && strcmp (vertex, "-") != 0)
            {
              fprintf (stderr, INPUT_ERROR_MSG);
              free_tree(new_tree,vertices_num);
              fclose (f);
              return FAIL;
            }
          vertex_val = (int) strtol (vertex, NULL, BASE);
          check_vertex_args(vertex_val, first, second,
                            legit_ptr1, legit_ptr2);
          if (vertex_val >= vertices_num)
            {
              fprintf (stderr, INPUT_ERROR_MSG);
              free_tree(new_tree,vertices_num);
              fclose (f);
              return FAIL;
            }
          ++children_counter;
          if(!add_node (new_tree, lines - 2,
                        vertex_val, children_counter))
            {
              free_tree(new_tree,vertices_num);
              fclose (f);
              return FAIL;
            }

          //update counter, go to next vertex
          ++counter;//seen nodes until now
          vertex = strtok (NULL, DELIMITER);
        }
    }
  if (counter + 1 != vertices_num
      || lines != vertices_num + 1)
    {
      fprintf (stderr, INPUT_ERROR_MSG);
      free_tree(new_tree,vertices_num);
      fclose (f);
      return FAIL;
    }

  //check if it's a tree
  if(!is_tree(new_tree, vertices_num))
    {
      fprintf (stderr, INPUT_ERROR_MSG);
      free_tree(new_tree,vertices_num);
      fclose (f);
      return FAIL;
    }
  Node* root = find_root(new_tree, vertices_num);

  if (root->key == first){
      legit_vertex1 = 1;
    }
  if (root->key == second){
      legit_vertex2 = 1;
    }

  if (legit_vertex1==0 || legit_vertex2==0)
    {
      fprintf (stderr, INPUT_ERROR_MSG);
      free_tree(new_tree,vertices_num);
      fclose (f);
      return FAIL;
    }
  bfs(new_tree, &root, vertices_num);
  int min_path = calc_min_path(new_tree, vertices_num);
  int max_path = calc_max_path(new_tree, vertices_num);
  int diameter = calc_diameter(new_tree, vertices_num, max_path);
  int* shortest_path = calc_shortest_path(first, second,
                                          new_tree, vertices_num);
  if (!shortest_path)
    {
      fprintf (stderr, ALLOC_ERROR_MSG);
      free_tree(new_tree,vertices_num);
      fclose (f);
      return FAIL;
    }
  printf("Root Vertex: %d\n", root->key);
  printf("Vertices Count: %d\n", vertices_num);
  printf("Edges Count: %d\n", vertices_num-1);
  printf("Length of Minimal Branch: %d\n", min_path);
  printf("Length of Maximal Branch: %d\n", max_path);
  printf("Diameter Length: %d\n", diameter);
  printf("Shortest Path Between %d and %d: ",
         first, second);
  for (int i = 0; i < new_tree[second]->dist; ++i)
    {
      printf("%d ", shortest_path[i]);
    }
  int j = new_tree[second]->dist;
  printf("%d\n", shortest_path[j]);
  free(shortest_path);
  free_tree(new_tree, vertices_num);
  //close the file
  fclose (f);
  return SUCCESS;
}

/**
 * main function
 * @param argc
 * @param argv
 * @return 1 upon success, 0 upon failure
 */
int main (int argc, char *argv[])
{
  //basic sanity tests for args
  if (argc != ARGS_NUM)
    {
      fprintf (stderr, ARGS_ERROR_MSG);
      return EXIT_FAILURE;
    }

  else if (!is_numeric (argv[SECOND_ARG])
           || !is_numeric (argv[THIRD_ARG]))
    {
      fprintf (stderr, INPUT_ERROR_MSG);
      return EXIT_FAILURE;
    }

  int first_vertex = (int) strtol (argv[SECOND_ARG], NULL, BASE);
  int second_vertex = (int) strtol (argv[THIRD_ARG], NULL, BASE);

  //build tree, perform tests for file
  if (!build_tree (argv[FIRST_ARG],
                   first_vertex,
                   second_vertex))
    {
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
