#define _GNU_SOURCE
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>

#include<assert.h>
#include<errno.h>
#include<ctype.h>

typedef void* Nullptr;                      //creating a nullptr
Nullptr nullptr=0;


#define ExampleFile "./wl.txt"
#define NoError 0                           //uk this
#define find_node(x) find_node_linear(x)
#define find_leaf(x,y) find_leaf_linear(x,y)
#define lookup(x,y) lookup_linear(x,y)
#define find_last(x) find_last_linear(x)    //called a macro....when the preprocessor encounters find_last in your code, it converts to find_last_linear
#define reterr(x) \
    errno = (x);  \
    return nullptr                          //whenever preprocessor encounters reterr(x) in your code...the error(errno.h) is equated to errno and it returns a nullptr to stop the code

//A macro which takes an arguement x with is a string then zeroes the buffer which acts a temporary variable an then copies the contents of x into the buffer and then puts it into a file using write() if the size of the string is not zero.

#define Print(x)  \
    zero(buf,256);  \
    strncpy((char *)buf,(char *)x,255);\
    size = (int16)strlen((char *)buf);\
    if(size) \
        write(fd,(char *)buf,size)

#define TagRoot 1
#define TagNode 2
#define TagLeaf 4                           //i am not writing 3 cause i will be using bitwise operations to check the tag and 3 is 1|2 so it will be confused with root and node

typedef unsigned int int32;                 //i am using unsigned cause there is no -ve value and i am using 32 bit cause i want to store large values
typedef unsigned short int int16;           //i am using unsigned short cause i want to store medium values
typedef unsigned char int8;                 //i am using unsigned char cause i want to store small values like key and value as it prevents usage of excess memory
typedef unsigned char Tag;                  //tells me whether a node is a root, node or leaf
typedef struct s_leaf
{
    Tag tag;
    union u_tree *west; //points to union because left can be just another leaf or aa node
    struct s_leaf *east;
    int8 key[128]; //key of leaf
    int8 *value; //address of value
    int16 size; //size of value
}Leaf;

typedef struct s_node
{
    Tag tag;
    struct s_node *north; //points to parent node
    struct s_node *west; //points to child node..here all everything left of a node is a child node and right is a leaf
    struct s_leaf *east; //leaf cause its pointing right
    int8 path[256]; //path of node ex. /home/hars_ha31/NUCLEAR CODES/C/PROJECTS/GOOD ONES/tree/tree.c

}Node;

typedef union u_tree
{
    Node n;
    Leaf l;
}Tree;


void zero(int8*, int16);
Node *create_node(Node*, int8*);
Leaf *find_last_linear(Node*);
Node *find_node_linear(int8*);
Leaf *find_leaf_linear(int8*,int8*);
int8 *lookup_linear(int8*,int8*);
Leaf *create_leaf(Node*,int8*, int8*, int16);
void print_tree(int, Tree*);
int8 *indent(int8);
Tree *example_tree(void);
int32 example_leaves(void);
int8 exapmple_path(int8);
int8 *example_duplicate(int8*);


