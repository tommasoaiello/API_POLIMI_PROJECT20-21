#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

//structs

typedef struct lint_s{
    unsigned long long int c;
    unsigned long long int n;
}lint;

typedef struct graph_node {
    lint* sum_cammini_minimi;
    unsigned long int index;
    struct graph_node *left, *right, *padre;
    bool is_red;
}rb_node;

typedef struct rb_tree_s{
    rb_node *root;
}rb_tree;

typedef struct node_and_edge_s{
    unsigned long long int dist_from_zero;
    unsigned long int numero_nodo;
}node_and_edge;

typedef struct heap_s{
    node_and_edge **priority_queue;
    unsigned long int max_size;
    unsigned long int size;
}heap;


unsigned long int d, k,counter;
unsigned long int** matrice_adiacenza;
rb_node *sentinella, *maxsum;
rb_tree *albero;

unsigned long int custom_atol_simple(char * token){
    unsigned long int num=0;
    while(!(token[counter]=='\n' || token[counter]=='\0')) {
        if(token[counter]==','){
            counter++;
            return num;
        }
        num= num*10 + token[counter] - '0';
        counter++;
    }

    return num;
}


node_and_edge * newElement(unsigned long long int dist, unsigned long int i){
    node_and_edge *nodo = malloc(sizeof(node_and_edge));
    nodo-> dist_from_zero = dist;
    nodo->numero_nodo = i;
    return nodo;
}



heap* heap_init(unsigned long int dimen){
    heap * heap1 = malloc(sizeof(heap));
    heap1 -> priority_queue = malloc(sizeof(node_and_edge*)*dimen);
    heap1 -> max_size = dimen;
    heap1 -> size = 0;
    return heap1;
}


void heap_swap(heap *heap1, unsigned long int i, unsigned long int j){
    node_and_edge *temporary;
    temporary = heap1 -> priority_queue[j];
    heap1->priority_queue[j]= heap1->priority_queue[i];
    heap1->priority_queue[i]= temporary;
    return;
}

unsigned long int heap_minChild(heap *heap1,unsigned long int i){
    unsigned long int left_child = 2*i+1;
    unsigned long int right_child = 2*i+2;
    if(right_child >= heap1->size)
        return left_child;
    return (heap1->priority_queue[left_child]->dist_from_zero < heap1->priority_queue[right_child]->dist_from_zero)? left_child:right_child;
}

void heap_heapify(heap* heap1, unsigned long int i){
    if(2*i+1 >= heap1->size)
        return;
    unsigned long int j = heap_minChild(heap1,i);
    if(heap1->priority_queue[i]->dist_from_zero > heap1->priority_queue[j]->dist_from_zero){
        heap_swap(heap1, i, j);
        heap_heapify(heap1,j);
    }

}

unsigned long int heap_decreaseKey(heap* heap1, unsigned long int i, unsigned long long int costo){
    unsigned long int parent=(i-1)/2;
    heap1->priority_queue[i]->dist_from_zero= costo;
    while(i!=0 &&  heap1->priority_queue[parent]->dist_from_zero > heap1->priority_queue[i]->dist_from_zero){
        heap_swap(heap1, i, parent);
        i=parent;
        parent= (i-1)/2;
    }
    return i;
}




node_and_edge* heap_deleteMin(heap* heap1){
    node_and_edge *ret= heap1->priority_queue[0];
    heap_swap(heap1, 0, heap1->size-1);
    if(heap1->size>0)
        heap1->size--;
    heap_heapify(heap1, 0);
    return ret;
}

unsigned long int heap_insert(heap *heap1, node_and_edge *element){
    
    unsigned long int i = heap1->size++;
    heap1->priority_queue[i] = element;
    i= heap_decreaseKey(heap1,i,element->dist_from_zero);
    return i;
}






lint* lint_sum(lint* num1, lint* num2, lint* res){
    unsigned long long int c1,c2,n1,n2,carry,sum;
    c1 = num1->c;
    c2= num2->c;
    n1= num1->n;
    n2= num2->n;
    sum = n1 + n2;
    carry= c1 + c2;

    if(sum < n1 || sum < n2){
        carry++;
    }

    res->c = carry;
    res->n = sum;
    return res;

}

int lint_compare(lint* num1, lint* num2){
    unsigned long long int c1,c2,n1,n2;
    c1 = num1->c;
    c2= num2->c;
    n1= num1->n;
    n2= num2->n;
    if(c1!=c2){
        return c1 > c2;
    }
    return n1 > n2;
}

lint* lint_init(unsigned long long int n){
    lint* ret= malloc(sizeof(lint));
    if(!ret) perror("cannot initialize lint");
    ret->n= n;
    ret->c=0;
    return ret;

}




rb_tree* rb_treeInit(rb_node *rootoo){
    rb_tree *tree;
    tree = malloc(sizeof(rb_tree));
    tree->root= rootoo;
    return tree;
}

rb_node * rb_init_sentinella(unsigned long int k){
    rb_node *sent;
    sent= malloc(sizeof(rb_node));
    sent->is_red=false;
    sent->sum_cammini_minimi= lint_init(0);
    sent->index = k+1;
    sent->left=sent->right=sent->padre=NULL;
    return sent;
}


rb_node* rb_newNode(lint *sum, unsigned long int indice)
{
    rb_node* temp = malloc(sizeof(rb_node));
    temp->sum_cammini_minimi = sum;
    temp->index = indice;
    temp->left = temp->right = temp->padre = sentinella;
    temp->is_red=true;
    return temp;
}


rb_tree* left_rotate(rb_tree* tree, rb_node* nodo){
    rb_node *y;
    y= nodo->right;
    nodo->right= y->left;

    if(y->left != sentinella)
        y->left->padre=nodo;
    y->padre=nodo->padre;

    if(nodo->padre==sentinella){
        tree->root = y;
    }
    else if(nodo==nodo->padre->left)
        nodo->padre->left=y;
    else
        nodo->padre->right=y;
    y->left = nodo;
    nodo->padre=y;

    return tree;
}

rb_tree * right_rotate(rb_tree* tree, rb_node* nodo){
    rb_node *y;
    y= nodo->left;
    nodo->left= y->right;

    if(y->right != sentinella)
        y->right->padre=nodo;
    y->padre=nodo->padre;

    if(nodo->padre==sentinella){
        tree->root = y;
    }
    else if(nodo==nodo->padre->right)
        nodo->padre->right = y;
    else
        nodo->padre->left = y;
    y->right = nodo;
    nodo->padre=y;

    return tree;
}


rb_tree * rb_insert_balance(rb_tree *tree, rb_node *nodo){
    rb_node *y;
    while(nodo->padre->is_red == true){
        if(nodo->padre == nodo->padre->padre->left){
            y = nodo->padre->padre->right;
            if(y->is_red == true){
                nodo->padre->is_red = false;
                y->is_red = false;
                nodo->padre->padre->is_red= true;
                nodo= nodo->padre->padre;
            }
            else if(nodo == nodo->padre->right){
                nodo = nodo->padre;
                tree = left_rotate(tree,nodo);
            }
            else{
                nodo->padre->is_red=false;
                nodo->padre->padre->is_red=true;
                tree = right_rotate(tree, nodo->padre->padre);
            }
        }
        else{
            y = nodo->padre->padre->left;
            if(y->is_red == true){
                nodo->padre->is_red = false;
                y->is_red = false;
                nodo->padre->padre->is_red= true;
                nodo = nodo->padre->padre;
            }
            else if(nodo == nodo->padre->left){
                nodo = nodo->padre;
                tree = right_rotate(tree,nodo);
            }
            else{
                nodo->padre->is_red=false;
                nodo->padre->padre->is_red=true;
                tree = left_rotate(tree, nodo->padre->padre);
            }
        }
    }
    tree->root->is_red = false;
    return tree;
}


rb_tree * rb_insert(rb_tree *tree, rb_node *nodo){
    rb_node *y = sentinella;
    rb_node *x = tree->root;
    while(x!= sentinella){
        y=x;
        if(lint_compare(x->sum_cammini_minimi,nodo->sum_cammini_minimi))
            x = x->left;
        else
            x = x->right;
    }
    nodo->padre = y;
    if(y==sentinella){
        tree->root = nodo;
    }
    else if(lint_compare(y->sum_cammini_minimi,nodo->sum_cammini_minimi))
        y->left = nodo;
    else
        y->right = nodo;
    nodo->left = sentinella;
    nodo->right = sentinella;
    nodo->is_red = true;

    tree = rb_insert_balance(tree, nodo);

    return tree;

}


rb_tree* rb_transplant(rb_tree* tree, rb_node* u, rb_node* v){
    if(u->padre==sentinella){
        tree->root = v;
    }
    else if(u==u->padre->left)
        u->padre->left = v;
    else
        u->padre->right=v;
    v->padre = u->padre;
    return tree;
}

rb_node* rb_minimum(rb_node* x){
    while(x->left != sentinella){
        x = x->left;
    }
    return x;
}

rb_node* rb_maximum(rb_node* x){
    while(x->right != sentinella){
        x = x->right;
    }
    return x;
}

rb_tree * rb_deleteFix(rb_tree* tree, rb_node *x){
    rb_node *w=NULL;
    while(x!=tree->root && x->is_red==false){
        if(x==x->padre->left){
            w= x->padre->right;
            if(w->is_red == true){
                w->is_red = false;
                x->padre->is_red=true;
                tree= left_rotate(tree,x->padre);
                w=x->padre->right;
            }
            if(w->left->is_red==false && w->right->is_red==false){
                w->is_red=true;
                x=x->padre;
            }
            else if(w->right->is_red==false){
                w->left->is_red= false;
                w->is_red=true;
                tree= right_rotate(tree,w);
                w= x->padre->right;
            }
            else{
                w->is_red= x->padre->is_red;
                x->padre->is_red=false;
                w->right->is_red=false;
                tree= left_rotate(tree,x->padre);
                x= tree->root;
            }
        }
        else{
            w= x->padre->left;
            if(w->is_red == true){
                w->is_red = false;
                x->padre->is_red=true;
                tree= right_rotate(tree,x->padre);
                w=x->padre->left;
            }
            if(w->right->is_red==false && w->left->is_red==false){
                w->is_red=true;
                x=x->padre;
            }
            else if(w->left->is_red==false){
                w->right->is_red= false;
                w->is_red=true;
                tree= left_rotate(tree,w);
                w= x->padre->left;
            }
            else{
                w->is_red= x->padre->is_red;
                x->padre->is_red=false;
                w->left->is_red=false;
                tree= right_rotate(tree,x->padre);
                x= tree->root;
            }
        }
    }
    x->is_red=false;
    return tree;
}

rb_tree * rb_deleteMax(rb_tree *tree, rb_node *nodo){
    rb_node *x = NULL;
    bool y_is_red = nodo->is_red;
    if(nodo->left == sentinella){
        x = nodo->right;
        tree = rb_transplant(tree, nodo, nodo->right);
    }
    else if(nodo->right == sentinella){
        x = nodo->left;
        tree = rb_transplant(tree, nodo, nodo->left);
    }

    free(nodo->sum_cammini_minimi);
    free(nodo);


    if(y_is_red == false)
        return rb_deleteFix(tree, x);
    return tree;
}



lint* dijikstra(unsigned long int ** matrice_adiacenza, unsigned long int d)
{
    unsigned long int i;
    unsigned long long int *distance;
    node_and_edge *vertice=NULL;
    unsigned long long int between_edges;
    heap *heap1 = heap_init(d);

    lint *sum, *tmp1, *tmp2;
    sum= lint_init(0);
    tmp1= lint_init(0);
    tmp2= lint_init(0);

  
    distance = (unsigned long long int*) malloc (sizeof(unsigned long long int)*d);

    


    distance[0]=0;
    heap_insert(heap1, newElement(distance[0],0));
    

    for(i=1; i < d; i++){
        distance[i]= ULLONG_MAX;
        heap_insert(heap1, newElement(distance[i],i));
    }



    while(heap1->size > 0){


        vertice=heap_deleteMin(heap1);

        
        if(vertice->dist_from_zero == ULLONG_MAX){
            free(vertice);
            
            while(heap1->size>0){
                vertice=heap_deleteMin(heap1);
                free(vertice);
            }
            break;
        }
        for(i=0; i < heap1->size; i++){

            if(matrice_adiacenza[vertice->numero_nodo][heap1->priority_queue[i]->numero_nodo] != 0 ){
                between_edges = vertice->dist_from_zero + matrice_adiacenza[vertice->numero_nodo][heap1->priority_queue[i]->numero_nodo];
                if(between_edges < distance[heap1->priority_queue[i]->numero_nodo]){
                    distance[heap1->priority_queue[i]->numero_nodo]=between_edges;
                    heap_decreaseKey(heap1,i,distance[heap1->priority_queue[i]->numero_nodo]);
                }

            }
        }
        free(vertice);
    }
    
    free(heap1->priority_queue);
    free(heap1);

   
    for(i=0;i < d;i++){
        if(distance[i]!= ULLONG_MAX){
            tmp1->n= distance[i];
            tmp2->c=sum->c;
            tmp2->n=sum->n;
            lint_sum(tmp1, tmp2, sum);
        }
    }
    free(distance);
    free(tmp1);
    free(tmp2);
    return sum;

}

void printTopK(rb_node* root){

    if (root != sentinella) {
        printTopK(root->left);
        if(maxsum->index == root->index)
            printf("%lu", root->index);
        else
            printf("%lu ", root->index);
        printTopK(root->right);

    }

}

void freeTree(rb_node* root){
    if (root != sentinella) {
        freeTree(root->left);
        freeTree(root->right);
        free(root->sum_cammini_minimi);
        free(root);
    }
}



int main() {
    char command[100];
    char *line_matrix=NULL;
    char separatore;
    lint* sum = NULL;
    unsigned long int number;
    unsigned long int i,j,g;

    unsigned long int count=0;
    unsigned long long int maxchar=0;


    g=0;
    matrice_adiacenza=NULL;
    counter=0;

    
    if(fgets(command, 100, stdin)!=NULL){
        sscanf(command, "%lu%c%lu" , &d, &separatore, &k);
        matrice_adiacenza = malloc(d*sizeof(unsigned long int*));
        for(i=0; i<d; i++){
            matrice_adiacenza[i]= malloc(d*sizeof(unsigned long int));
        }
        line_matrix= (char*) malloc((d*11)*sizeof(char));
        maxchar = d*11;
    }

    sentinella= rb_init_sentinella(k+1);
    albero =rb_treeInit(sentinella);
    maxsum = NULL;

    while(!feof(stdin)){
        if(fgets(command,100,stdin)!=NULL){
            if(command[0]=='A'){
                for(i=0; i<d; i++){
                    counter=0;
                    if(fgets(line_matrix,maxchar,stdin)!=NULL){
                        for(j=0; j<d; j++){
                            number = custom_atol_simple(line_matrix);
                            matrice_adiacenza[i][j] = (unsigned long int) number;
                        }
                    }
                }
                
                sum = dijikstra(matrice_adiacenza,d);
                if(count < k){
                    if (count == 0){
                    maxsum = rb_newNode(sum, count);
                    albero = rb_insert(albero, maxsum);
                    } 
                    else {
                    rb_node* tmp = rb_newNode(sum, count);
                    albero = rb_insert(albero, tmp);
                    if(!lint_compare(maxsum->sum_cammini_minimi, sum)){
                        maxsum = tmp;
                    }
                    }
                }
                else 
                    if(lint_compare(maxsum->sum_cammini_minimi,sum)){
                        albero = rb_deleteMax(albero,maxsum);
                        albero = rb_insert(albero, rb_newNode(sum, count));
                        maxsum = rb_maximum(albero->root);
                    } 
                
                count++;
            }
            else{
                if(g!=0)
                    printf("\n");
                if(albero->root!= sentinella){
                    printTopK(albero->root);
                }
                g++;
            }
        }

    }
    for(i=0; i<d; i++){
        free(matrice_adiacenza[i]);
    }
    
    
   
    return 0;
}