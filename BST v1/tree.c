#include "tree.h"
/*

typedef struct s_node
{
    Tag tag;
    struct s_node *north; //points to parent node
    struct s_node *west; //points to child node..here all everything left of a node is a child node and right is a leaf
    struct s_leaf *east; //leaf cause its pointing right
    int8 path[256]; //path of node ex. /home/hars_ha31/NUCLEAR CODES/C/PROJECTS/GOOD ONES/tree/tree.c
    
}Node;
*/


Tree root = {.n = {                       //accessing node within union tree and then accessing struct node... .n is just another way of writng root.n and n.tag...so instead of writing root.n.tag i can just write .tag and it will be same as root.n.tag
    .tag = (TagRoot | TagNode),           //as this is a root node the tag is both 1 and 2 which is 3
    .north = (Node *)&root,               //points to address of root(itself) which is of type node 
    .west = 0,                            //initialized to zero
    .east = 0,                            
    .path = "/"}};                        //for now its just /...


int8 *indent(int8 n)                      //this funtion returns a string which has n*2 empty spaces
{
    int16 i;                              //used as a loop counter
    static int8 buf[256];                 //string which u r returning...static because it needs to be a open variable so others can use it
    int8 *p;                              
    if(n<1)                               //if n is less than 1 i will be returning an empty string
        return (int8 *)"";
    assert(n<120);                        //n needs to be approximately less than 120 cause buf size is 256 and i dont wanna overflow it
    zero(buf,256);                        //zeroes the buff string
    for(i=0,p=buf;i<n;i++,p+=2)           //in every iteration...a string containing 2 spaces is copied into p the p pointer moves 2 steps ahead
    {
        strncpy((char *)p,"  ",2);
    }
    return buf;
}


void zero(int8 *str, int16 size)          //zeroes the memory allocated
{
    int8 *p;                              //*p holds address of str later used in for loop
    int16 n;                              //n is for size

    for(n=0,p=str;n<size;p++,n++)
    {
        *p=0;
    }
    return;

}


void print_tree(int fd, Tree *_root)                                    //a pretty printer
{
    int8 indentation=0;                                                 //keeps count of the number of indentations to print in each line..0th line - 0,1st line - 2, 2nd line - 4.......
    int8 buf[256];                                                      //buffer string which stores the value before writing to file
    int16 size;                                                         //translates to size of string being stored in buffer
    Node *n;                                                            //pointer which points to each node while iterating using for loop
    Leaf *l, *last;                                                     //common sense!
     
    for(n = (Node *)_root;n;n=n->west)                                  //here i am not using n->west cause...it wont check the last node as n->west is null
    {
        Print(indent(indentation++));                                   //indent() returns a string of spaces which i will Print in the buffer string and send it to the file
        Print(n->path);                                                 //n->path is printed in the buffer string and sent to the file
        Print("\n");                                                    //upar dekh!

        if(n->east)                                                     //checks if node contains a leaf
        {
            last = find_last(n);                                        //finds the last leaf
            if(last)
            {
                for(l=last;(Node *)l != n;l=(Leaf *)l->west)
                {
                    Print(indent(indentation));                         //same as above
                    Print(n->path);
                    Print("/");
                    Print(l->key);
                    Print("-> '");
                    write(fd, (char *)l->value, (int)l->size);
                    Print("'\n");
                }
            }    
        }
    }
    return;
}


Node *create_node(Node *parent, int8 *path)            //funtion to create a node and return the address of the node created
{
    Node *n;                                           //*n is a pointer to struct node which will point to the address of the node being created
    int16 size;                                        //size of node to be created
    errno = NoError;
    assert(parent);                                    //checks if parent is there...if not it will terminate the program
    size = sizeof(Node);                               //size of node to be created is equal to size of struct node
    n= (Node *)malloc((int)size);                      //typecasting size to int because address might be in the form of 0x1123ed78, using malloc because each node neeeds to have seperate block of memory
    zero((int8 *)n, size);                             //zeroing the memory allocated to node so that it does not contain any garbage values...because we are not using calloc
    parent->west = n;                                  //(*parent).west...west pointer of struct node is made to point to the new node
    n->tag = TagNode;                                  //(*n).tag = n.tag....remeber n is an address
    n->north = parent;                                 //n.pzrent...read the above comment
    strncpy((char *)n->path,(char *)path,255);         //copies path to the path of node n
    return n;                                          //return address of n
}


Leaf *find_leaf_linear(int8 *path,int8 *key)
{
    Node *n;
    Leaf *l, *ret;
    n=find_node(path);
    if(!n)
    {
        return (Leaf *)0;
    }

    for(ret=(Leaf *)0,l=n->east;l;l=l->east)
    {
        if(!strcmp((char *)l->key,(char *)key))
        {
            ret = l;
            break;
        }
    }
    return ret;
}


int8 *lookup_linear(int8 *path, int8 *key)                           //returns only the value and not the entire leaf
{
    Leaf *p;
    p = find_leaf_linear(path,key);
    return (p)?p->value:(int8 *)0;
}


Node *find_node_linear(int8 *path)
{
    Node *p,*ret;
    for(ret=(Node *)0,p = (Node *)&root;p;p=p->west)
    {
        if(!strcmp((char *)p->path,(char *)path))
        {
            ret=p;
            break;
        }
        
    }
    return ret;

}


Leaf *find_last_linear(Node *parent)                   //funtion to find if leaf is first for its node or nth leaf
{
    assert(parent);
    Leaf *l;                                           //creates an address for the leaf which is *l
    errno = NoError;                                   
    if(!parent->east)                                  //checks if the new leaf which is being added is the first leaf or not
    {
        return (Leaf *)0;                              //CHANGED !!!!//USING A MACRO!!! go to tree.h file.
    }
    for(l = parent->east; l->east; l=l->east);        //if not the first leaf it iterates through where l->east is a condition...if l->east exists continue
    assert(l);                                        //acknowledge the last leaf
    return l;
}


Leaf *create_leaf(Node *parent,int8 *key, int8 *value, int16 count)             //funtion to create a leaf
{
    assert(parent);                                                             //leaf can be created only when parent is there
    Leaf *l,*new;                                                               //2 pointer variables(l will be used to find the previous element)
    int16 size;                                                                 //size of leaf
    size = sizeof(Leaf);
    new = (Leaf *)malloc((int)size);                                            //returns address for newly created leaf
    assert(new);

    l = find_last(parent);                                                      //finds the previous leaf        
    if(!l)                                                                      //if no leaf is present do the below code
        parent->east = new;
    else
        l->east = new;                                                          //else do this
    zero((int8 *)new, size);                                                    //zeroing the space
    new->tag = TagLeaf;                                                         //tag 4
    new->west=(!l) ? (Tree *)parent : (Tree *)l;                                //the west pointer points to tree if l doesnt exist else it points to l which is previous leaf
    strncpy((char *)new->key, (char *)key,127);                                 //key being copied
    new->value = (int8 *)malloc(count);                                         //clears space for value
    zero(new->value,count);                                                     //uk it already!
    assert(new->value);
    strncpy((char *)new->value, (char *)value,count);
    new->size = count;

    return new;
}

Tree *example_tree()                                                           //yeah just prints one so that i can test
{
    int8 c;                                                                    //char n
    Node *n,*p;
    int8 path[256];
    int32 x;                                                                   //stores the value or wtever u call it

    zero(path,256);
    x=0;


    for(n=(Node *)&root,c='a';c<='z';c++)                                      //n is initialized to n and c to the alphabet a in the start and c iterates till z
    {
        x=(int32)strlen((char *)path);                                         //used to tell where the current strong ends so a new character can be added right after it
        *(path + x++) = '/';                                                   //right after the string ends we add a '/' then the following node
        *(path + x) = c;                                                       //the following node
        printf("%s\n",path);                                                   //remember we didnt specify a null character..so this prints the path and goes to the nexxt line

        p=n;                                                                   //now p is equated to the current node n and is used a s the parent node for the next node
        n=create_node(p, path);                                                //funtion to create the next node
    }

    return (Tree *)&root;
     
}


int8 *example_path(int8 path)                                                  //helps to visualise how a path works.............suppose path is c...then the funtion returns "a/b/c"
{
    int32 x;
    static int8 buf[256];
    int8 c;
    zero(buf,256);

    for(c='a';c<=path;c++)
    {
        x=(int32)strlen((char *)buf);
        *(buf + x++) = '/';
        *(buf + x) = c;
    }

    return buf;

}


int8 *example_duplicate(int8 *str)
{
    int16 n,x;
    static int8 buf[256];
    zero(buf,256);
    strncpy((char *)buf,(char *)str,255);
    n = (int8)strlen((char *)buf);
    x = (n*2);

    if(x>254)
        return buf;
    else
        strncpy((char *)buf+n, strdup((char *)buf), 255-n);
        return buf;

}


int32 example_leaves()
{
    //int fd;
    FILE *fd;
    int32 x,y;
    int8 c;
    int8 buf[256];
    int8 *path , *val;
    Leaf*l;
    Node *n;

    //fd = open(ExampleFile, O_RDONLY);                                         //opens an exampple file in read only mode
    fd = fopen(ExampleFile, "r");
    assert(fd);
    zero(buf,256);
    y=0;


    while(fgets((char *)buf, 255, fd))
    {
        x = (int32)strlen((char *)buf);
        *(buf+x-1)=0;                                                           //makes the last character of string as /0
        path = example_path(*buf);                                              //suppose buff is c....then path would be a/b/c
        n = find_node(path);                                                    //finds a node with the following path
        if(!n)
        {
            zero(buf, 256);
            continue;                                                           //skips and looks at the next 255 characters if n is null
        }

        val = example_duplicate(buf);

#if 0
        printf("\n");
        printf("node = '%s'\n",n->path);
        printf("buf = '%s'\n", buf);
        printf("val = '%s'\n", val);
        printf("len = '%d'\n", (int16)strlen((char *)val));
        printf("\n");
#endif 
        l = create_leaf(n, buf, val,(int16)strlen((char *)val));
        y++;
    }

    fclose(fd);
    return y;

}

int main()
{
    Tree *example;
    int32 x;
    int8 *p;


    example = example_tree();
    printf("populating tree....");
    fflush(stdout);
    x = example_leaves();
    printf("done %d \n\n",x);
    print_tree(1,example);
    return 0;
}
