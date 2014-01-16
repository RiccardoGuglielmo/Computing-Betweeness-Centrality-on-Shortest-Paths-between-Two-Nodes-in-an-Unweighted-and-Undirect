// Librerie:
#include <stdio.h>
#include <stdlib.h>

// Definizione delle strutture dati:
typedef struct vert
{
    int val;
    struct vert *next;
}vertix;

typedef struct
{
    int  dist;
    int  path1;
    int  path2;
    char color;
}node;

// Dichiarazione dei prototipi delle funzioni:
char createGraph ( vertix *graph, char *filename );
void bfs1 ( node *v, unsigned int v1, unsigned int dim, vertix *graph );
void bfs2 ( node *v, unsigned int v2, unsigned int dim, vertix *graph );
vertix *push ( vertix *coda, unsigned int val );
vertix *pop ( vertix *coda );

// Funzione Principale:
int main ( int argc, char *argv[] )
{
    // Controllo del numero di parametri ricevuti:
        
    if ( argc != 4 )
    {
		printf ( "Usage: max_betweeness <graph> <node index> <node index>\n" );
		return 1;
	}
	
	
    // Dichiarazione variabili e inizializzazione:
        
    unsigned int v1 = atoi(argv[2]), v2 = atoi(argv[3]);
    printf("FILENAME: %s SOURCE INDEX: %d TARGET INDEX: %d\n",argv[1],v1,v2);
    if ( v1 == v2 )          //Controlla se v1 e v2 sono lo stesso nodo.
       return 0;
    FILE *f = fopen ( argv[1], "r" );
    if ( f == NULL )
    {
        printf ( "Impossibile aprire il file!" );
        return 1;
    }
    const unsigned int dim;
    fscanf ( f, "%d\n", &dim );
    fclose(f);
    if ( dim < 2 ) return 1;
    vertix graph[dim];
    int i;
    for( i = 0; i < dim; i++ )
    {
        graph[i].val = i;
        graph[i].next = NULL;
    }
    if ( createGraph ( graph, argv[1] ) )
        return 1;
    
    
    // Conteggio betweenness score dei nodi fra v1 e v2:

    vertix *temp = NULL;
    temp = &graph[v1];
    while ( temp != NULL )
    {
        if( temp->val == v2 )
        return 0;
        temp = temp->next;
    }
    
    node v[dim];
    for( i = 0; i < dim; i++ )
    {
        v[i].dist  = -2 ;
        v[i].path1 =  0 ;
        v[i].path2 =  0 ;
	    v[i].color = 'w';
    }
    
    bfs1 ( v, v1, dim, graph );
    
    if ( v[v2].dist == -2 )  //Controlla se esiste un cammino fra v1 e v2.
       return 0;
       
    bfs2 ( v, v2, dim, graph );
    
    
    // Calcolo dei betweenness score:
    int bsMAX = 0;
    for ( i = 0; i < dim; i++ )
       if ( ( v[i].path1 *= v[i].path2 ) > bsMAX )
          bsMAX = v[i].path1;
    
    
    // Stampa a video del/dei beteweenness score massimo/i (se ci sono):
    for ( i = 0; i < dim; i++ )
        if ( v[i].path1 == bsMAX )
           printf ( "%d ", i );
    printf ( "\n" );
    
    
    return 0;
}

/* Creazione del grafo, utilizzando una lista di adiacenza:  */
char createGraph ( vertix *graph, char *filename )
{
    unsigned int i, j;
    FILE *f = fopen ( filename, "r" );
    fscanf ( f, "%d\n", &i );
    
    vertix *aux = NULL;
    vertix *temp = NULL; 
    while ( ! feof(f) )
    {
        fscanf ( f, "%d %d\n", &i, &j );
        
        aux = (vertix*)malloc(sizeof(vertix));
        if(aux == NULL)
        {
            printf ( "Memoria esaurita!\n" );
            return 1;
        }
        aux->val = j;
        aux->next = NULL;
        temp = &graph[i];
        while(temp->next != NULL)
            temp = temp->next;
        temp->next = aux;

        aux = (vertix*)malloc(sizeof(vertix));
        if(aux == NULL)
        {
            printf("Memoria esaurita!\n");
            return 1;
        }
        aux->val = i;
        aux->next = NULL;
        temp = &graph[j];
        while(temp->next != NULL)
            temp = temp->next;
        temp->next = aux;
    }
    
    fclose(f);
    
    return 0;
}

/* BFS1 - Visita in ampiezza con memorizzazione nel vettore "v" di strutture
*  "vertice", della distanza (numero di archi) di ogni nodo, dal nodo v1
*   ricevuto come parametro e del numero di cammini che lo raggiungono.    */
void bfs1 ( node *v, unsigned int v1, unsigned int dim, vertix *graph )
{
    vertix *coda = NULL;
    vertix *temp = NULL;
    v[v1].dist = 0;
    coda = push ( coda, v1 );
    while ( coda != NULL )
    {
        temp = graph[coda->val].next;
        while(temp != NULL)
        {
            if( v[temp->val].dist == -2 )
            {
                v[temp->val].dist = v[coda->val].dist + 1;
                coda = push ( coda, temp->val );
            }
            if ( coda->val == v1 ) v[temp->val].path1 = 1;
            else if ( ( v[temp->val].dist > v[coda->val].dist ) || ( v[coda->val].dist == -2 ) )
                     v[temp->val].path1 += v[coda->val].path1;
            temp = temp->next;
        }
        coda = pop(coda);
    }
}

/* BFS2 - Visita in ampiezza con memorizzazione nel vettore "v" di strutture "vertice", del
*   numero di cammini che lo raggiungono, a partire dal nodo  v2 ricevuto come parametro.  */
void bfs2 ( node *v, unsigned int v2, unsigned int dim, vertix *graph )
{
    vertix *coda = NULL;
    vertix *temp = NULL;
    v[v2].color = 'b';
    coda = push ( coda, v2 );
    while ( coda != NULL )
    {
        temp = graph[coda->val].next;
        while(temp != NULL)
        {
            if ( v[temp->val].dist == v[coda->val].dist - 1 )
            {
                if ( v[temp->val].color == 'w' )
                {
                    coda = push ( coda, temp->val );
                    v[temp->val].color = 'b';
                }
                if ( coda->val == v2 ) v[temp->val].path2 = 1;
                else v[temp->val].path2 += v[coda->val].path2;
            }
            temp = temp->next;
        }
        v[coda->val].color = 'b';
        coda = pop(coda);
    }
}

/* PUSH - Inserimento in una coda di una struttura nodo della quale
*   si riceve come parametro, un valore da memorizzare in essa.   */
vertix *push ( vertix *coda, unsigned int val )
{
    vertix *temp;
    if(coda==NULL)
    {
        coda=(vertix*)malloc(sizeof(vertix));
        if(coda==NULL) printf("\n\n\t Memoria esaurita!");
        coda->val=val;
        coda->next=NULL;
    }
    else
    {
        temp=coda;
        while(temp->next!=NULL) temp=temp->next;
        temp->next=(vertix*)malloc(sizeof(vertix));
        temp=temp->next;
        if(temp==NULL) printf("\n\n\t Memoria esaurita!");
        temp->val=val;
        temp->next=NULL;
    }
    return coda;
}

/* POP - Estrazione di una struttura nodo dalla coda,
*   che viene poi restituita alla funzione chiamante.  */
vertix *pop ( vertix *coda )
{
    vertix *temp;
    if(coda==NULL) return(NULL);
    temp=coda;
    coda=coda->next;
    free(temp);
    return coda;
}
