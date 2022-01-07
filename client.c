// PROJET LABYRINTHE
// ANTONIN DOLLMANN ET HUGOIS GANTOIS TP1B
/*
EXTRAIT VALGRIND
==15477== HEAP SUMMARY:
==15477==     in use at exit: 0 bytes in 0 blocks
==15477==   total heap usage: 6,995 allocs, 6,995 frees, 160,884 bytes allocated
==15477==
==15477== All heap blocks were freed -- no leaks are possible
==15477==
==15477== For lists of detected and suppressed errors, rerun with: -s
==15477== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)

comme vous pouvez le voir même sur de petit labyrinthe le programme alloue énormement de bytes
la raison : beaucoup trop de node et de edge pour le bellmanford
la solution : réduire le nombre de node en ne rejoutant uniquement les node nécessaire
problème : cela implique de remanier l'entiéreté du tableau et des node ce qui nous a pris beaucoup de temps a faire a la base
la solution la donc pas été appliqué étant donnée que l'algorithme arrive a battre labyrinth-10 et labyrinth-15 dans la plupart des cas
AVERTISSEMENT : le programme est lent pour finir une partie.. sur un ryzen 5 3600 on avoisinne la minute
*/


#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#define BUFSIZE 256

//on déclare quelque variable global pour pouvoir les modifier simplement dans une fonction de type void
char *rep = '\0';
int countBellFord = 0;
char *path = NULL;

//on déclare quelque structure très utile
//pave pour case car cce mot est déja utiliser par le language C dans un switch
struct pave {
  size_t marked;
  char value;
  size_t distance_to_treasure;
};

struct save {
  struct pave **data;
  int width;
  int height;
};

struct joueur{
  int posx_init;
  int posy_init;
  int posx_treasure;
  int posy_treasure;
  int currx;
  int curry;
};

struct Edge {
  int u;
  int v;
  int w;
};

struct Graph {
  int nbNode;
  int nbEdge;
  struct Edge **edge;
};

//crée une case
struct pave *create_pave(){
  struct pave *curr = malloc(sizeof(struct pave));
  curr->marked = 0;
  curr->value = '0';
  return curr;
}

//crée le tableau on initialisent chaque case avec ces valeur mak 'carac' et distance_to_treasure
void create_save(struct save *self, struct joueur *player){
  for (size_t i = 0; i < self->height; i++) {
    for (size_t j = 0; j < self->width; j++) {
      self->data[i*self->width+j] = create_pave();
      self->data[i*self->width+j]->distance_to_treasure = sqrt((player->posx_treasure-j)*(player->posx_treasure-j) + (player->posy_treasure-i)*(player->posy_treasure-i));
    }
  }
}

//permet de détruire tout les chemin du graph
void destroy_node(struct Graph *graph){
  for (size_t i = 0; i < graph->nbEdge; i++) {
    free(graph->edge[i]);
  }
  free(graph->edge);
}

//permet de détruire toute les calloc / malloc
void destroy(struct save *tab,struct joueur *player,struct Graph *graph, char *path){
  for (size_t i = 0; i < tab->width*tab->height; i++) {
    free(tab->data[i]);
  }
  destroy_node(graph);
  free(graph);
  free(path);
  free(tab->data);
  free(tab);
  free(player);
}

// on cherche a sauvegarder chaque information que nous donne le buf
void save_information(char *buf,struct save *tab,struct joueur *player){
  int place = 0;
  for (int y = -1; y < 2; y++) {
    for (int x = -1; x < 2; x++) {
      // si y == 0 et x == 0 alors on est sur la case du personnage et donc on la mark
      if (y == 0 && x == 0) {
        tab->data[player->currx+(player->curry*tab->width)]->marked++;
        tab->data[player->currx+(player->curry*tab->width)]->value = 'P';
        //sinon si elle est bien dans les dimension du labyrinthe on lui affecte ces nouvelle donnée
      }else{
        if (player->currx+x >= 0 && player->curry+y >= 0 && player->currx+x < tab->width && player->curry+y < tab->height) {
          tab->data[(player->currx+x) + (player->curry+y)*tab->width]->value = buf[place];
        }
        place++;
      }
    }
  }
}

// permet de réprésentée archaiquement le labyrinthe
void print_tab_in_error(struct save *tab){
  fprintf(stderr, "00|");
  for (size_t l = 0; l < tab->width; l++) {
    if (l < 10) {
      fprintf(stderr, "0%li|", l);
    }else{
      fprintf(stderr, "%li|", l);
    }
  }
  fprintf(stderr, "\n");
  for (size_t j = 0; j < tab->height; j++) {
    if (j < 10) {
      fprintf(stderr, "0%li|", j);
    }else{
      fprintf(stderr, "%li|", j);
    }
    for (size_t i = 0; i < tab->width; i++) {
      fprintf(stderr, "%c%c|", tab->data[j*tab->width+i]->value,tab->data[j*tab->width+i]->value);
    }
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "\n");
  fprintf(stderr, "\n");
}

// permet de réprésentée archaiquement la distance jusqu'au tresor
void print_distance_to_in_error(struct save *tab){
  for (size_t j = 0; j < tab->height; j++) {
    fprintf(stderr, "%li|", j);
    for (size_t i = 0; i < tab->width; i++) {
      if (tab->data[j*tab->width+i]->distance_to_treasure < 10) {
        fprintf(stderr, "%li$|", tab->data[j*tab->width+i]->distance_to_treasure);
      }else{
        fprintf(stderr, "%li|", tab->data[j*tab->width+i]->distance_to_treasure);
      }

    }
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "\n");
  fprintf(stderr, "\n");
}

// permet de réprésentée archaiquement le case marked
void print_marked_to_in_error(struct save *tab){
  for (size_t j = 0; j < tab->height; j++) {
    fprintf(stderr, "%li|", j);
    for (size_t i = 0; i < tab->width; i++) {
      if (tab->data[j*tab->width+i]->marked == 0) {
        fprintf(stderr, "..|");
      }else{
        fprintf(stderr, "%li$|", tab->data[j*tab->width+i]->marked);
      }

    }
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "\n");
  fprintf(stderr, "\n");
}

//on iinitialise un chemin
struct Edge *creat_edge(int origin, int dest, int weight){
  struct Edge *edge = malloc(sizeof(struct Edge));
  edge->u = origin;
  edge->v = dest;
  edge->w = weight;
  return edge;
}


//on crée simplement un chemin
void creat_graph_partial(struct Graph *graph,struct save *tab, int origin, int dest, int count_edge, int max){
  //si on peut allez sur cette case alors le poid est 1
  if (tab->data[dest]->value == 'P' || tab->data[dest]->value == '_' || tab->data[dest]->value == '0') {
    graph->edge[count_edge] = creat_edge(origin,dest,1);
    // sinon c'est un mur ou une bordure ,le poid est de max
  }else{
    graph->edge[count_edge] = creat_edge(origin,dest,max);
  }
}

// on crééais un graph avec autant de node que notre tableau et pour chaque node 4 chemin NEWS (north east west south)
// on cherche ici a avoir le moins de pas possible et pas la meilleur optimisation espace/temps
// toutefois on pourrais clairement crée un graph avec uniquement les case nécessaire tout en ajoutant a chaque cas son id
// plutot que d'avoir node == case
// apres plusieur dizaine de test le problème d'optimisation viens clairement de cette fonction et de update qui sont bien trop lourd une fois arrive aux bellmanford
//Il faudrait revisiter l'intégralité de la création du tableau de base et donc du graph
void create_graph(struct Graph *graph,struct save *tab){
  graph->nbNode = tab->width*tab->height;
  graph->edge = calloc(graph->nbNode*4,sizeof(struct Edge *));

  //on met le max a une valeur qui ne peut pas être atteinte par le programme
  int max = graph->nbNode*2;
  int count_edge = 0;
  //on substitue dest et origin pour le pas avoir a faire le calcul y*tab->width ... etc  dans chaque condition a chaque fois
  int dest,origin;

  for (int y = 0; y < tab->height; y++) {
    for (int x = 0; x < tab->width; x++) {

      origin = y*tab->width+x;

      // si la case de départ est P _ 0 alors on fixe les noeud avec une valeur de base a 1
      if (tab->data[origin]->value == 'P' || tab->data[origin]->value == '_' || tab->data[origin]->value == '0') {

        //on crée le chemin nord
        dest = (y-1)*tab->width+x;
        if (dest >= 0 && y*tab->width >= dest) {
          creat_graph_partial(graph,tab,origin,dest,count_edge,max);
          count_edge++;
          //sinon c'est une bordure donc on max
        }else{
          graph->edge[count_edge] = creat_edge(origin,origin,max);
          count_edge++;
        }

        //on crée le chemin est
        dest = y*tab->width+x+1;
        if (dest < tab->height*tab->width && x+1 < tab->width) {
          creat_graph_partial(graph,tab,origin,dest,count_edge,max);
          count_edge++;
        }else{
          graph->edge[count_edge] = creat_edge(origin,origin,max);
          count_edge++;
        }

        //on crée le chemin ouest
        dest = y*tab->width+x-1;
        if (dest >= 0 && x-1 >= 0) {
          creat_graph_partial(graph,tab,origin,dest,count_edge,max);
          count_edge++;
        }else{
          graph->edge[count_edge] = creat_edge(origin,origin,max);
          count_edge++;
        }

        //on crée le chemin sud
        dest = (y+1)*tab->width+x;
        if (dest < tab->height*tab->width && y+1 < tab->height) {
          creat_graph_partial(graph,tab,origin,dest,count_edge,max);
          count_edge++;
        }else{
          graph->edge[count_edge] = creat_edge(origin,origin,max);
          count_edge++;
        }

      //sinon c'est que l'on ce trouve sur un mur et donc chaque chemin mène a max pour éviter de rentrer dans un mur
      }else{
        graph->edge[count_edge] = creat_edge(origin,(y-1)*tab->width+x,max);
        count_edge++;
        graph->edge[count_edge] = creat_edge(origin,y*tab->width+x+1,max);
        count_edge++;
        graph->edge[count_edge] = creat_edge(origin,y*tab->width+x-1,max);
        count_edge++;
        graph->edge[count_edge] = creat_edge(origin,(y+1)*tab->width+x,max);
        count_edge++;
      }

    }
  }
  //on attribue au nombre de chemin notre compteur (toujours égale a nbNode*4 je crois)
  graph->nbEdge = count_edge;
}



//on update un chemin d'un noeud avec sont nouveaux poids
void update_partial_two(struct Graph *graph,struct save *tab, int origin, int dest, int count_edge,int min, int max){
  if (tab->data[dest]->value == 'P' || tab->data[dest]->value == '_' || tab->data[dest]->value == '0' || tab->data[dest]->value == 'T') {
    graph->edge[count_edge]->w = min;
  }else{
    graph->edge[count_edge]->w = max;
  }
}

//on update chaque chemin d'un noeaud
int update_partial_one(struct Graph *graph,struct save *tab, int origin, int count_edge, int max,int x,int y){
  int dest;

  //on vérifie que l'on ne ce toruve pas en bordure sinon le poid deviens max
  //NORD
  dest = (y-1)*tab->width+x;
  if (dest >= 0 && y*tab->width >= dest) {
    update_partial_two(graph,tab,origin,dest,count_edge,1,max);
    count_edge++;
  }else{
    update_partial_two(graph,tab,origin,origin,count_edge,max,max);
    count_edge++;
  }

  //EST
  dest = y*tab->width+x+1;
  if (dest < tab->height*tab->width && x+1 < tab->width) {
    update_partial_two(graph,tab,origin,dest,count_edge,1,max);
    count_edge++;
  }else{
    update_partial_two(graph,tab,origin,origin,count_edge,max,max);
    count_edge++;
  }

  //OUEST
  dest = y*tab->width+x-1;
  if (dest >= 0 && x-1 >= 0) {
    update_partial_two(graph,tab,origin,dest,count_edge,1,max);
    count_edge++;
  }else{
    update_partial_two(graph,tab,origin,origin,count_edge,max,max);
    count_edge++;
  }

  //SUD
  dest = (y+1)*tab->width+x;
  if (dest < tab->height*tab->width && y+1 < tab->height) {
    update_partial_two(graph,tab,origin,dest,count_edge,1,max);
    count_edge++;
  }else{
    update_partial_two(graph,tab,origin,origin,count_edge,max,max);
    count_edge++;
  }


  return count_edge;
}

//on update le graph avec les nouvelle valeur
void update(struct Graph *graph,struct save *tab,struct joueur *player, int onlymarked){
  int count_edge = 0;
  int max = graph->nbNode*2;
  int origin;
  for (int y = 0; y < tab->height; y++) {
    for (int x = 0; x < tab->width; x++) {

      origin = y*tab->width+x;
      //on ne prend que les case marqué (uniquement sur le retour)
      if (onlymarked == 1) {
        //on vérifie que la case est marqué ainsi que la value P _ T UNIQUEMENT
        if (tab->data[origin]->marked > 0 && (tab->data[origin]->value == 'P' || tab->data[origin]->value == '_' || tab->data[origin]->value == 'T')) {

          count_edge = update_partial_one(graph,tab,origin,count_edge,max,x,y);

        }else{
          //sinon c'est soit un case non découverte sois un mur ou une bordure ce qui ne nous intéresse pas donc on met le poid a max
          for (size_t i = 0; i < 4; i++) {
            graph->edge[count_edge]->w = max;
            count_edge++;
          }
        }
      }else{
        //on vérifie que la value P _ 0 T
        if (tab->data[origin]->value == 'P' || tab->data[origin]->value == '_' || tab->data[origin]->value == '0' || tab->data[origin]->value == 'T') {

          count_edge = update_partial_one(graph,tab,origin,count_edge,max,x,y);

        }else{
          //sinon c'est un mur ou une bordure
          for (size_t i = 0; i < 4; i++) {
            graph->edge[count_edge]->w = max;
            count_edge++;
          }
        }
      }
    }
  }
}

//on traduit le tableau de predesseceur en direction NEWS
// permet de traduire le tableau solution en chaine solution
char *translate_path(int s[], int count){
  char *path = calloc(count,sizeof(char));
  int lenght = 0;
  // si on a une longueur de tableau de 1 alors il n'y a juste pas de tableau et le bellmanford a planté
  if (count <= 1) {
    path[0] = '\0';
    return path;
  }

  for (size_t i = count-2; i != 0; i--) {
    //x+1 si a l'est
    if (s[i+1] == s[i]+1) {
      path[lenght] = 'W';
      lenght++;
    }else{
      //x-1 si a l'ouest
      if (s[i+1] == s[i]-1) {
        path[lenght] = 'E';
        lenght++;
      }else{
        //inférieur si plus haut dans le tableau donc nord
        if (s[i+1] > s[i]) {
          path[lenght] = 'N';
          lenght++;
        }else{
          // supérieur si plus bas dans le tableau donc sud
          if (s[i+1] < s[i]) {
            path[lenght] = 'S';
            lenght++;
          }
        }
      }
    }
  }
  path[lenght] ='\0';
  fprintf(stderr, "%s\n",path );
  return path;
}


// ceci est un classique bellmanford avec un array de predecessor comme on peut en trouver partout sur le net (stackoverflow et programiz.com)
// ATTENTION ON A MODIFIER LA FIN DE LALGO
char *bellmanford(struct Graph *g, int source, int dest) {

  int i, j, u, v, w;
  int tV = g->nbNode;
  int tE = g->nbEdge;
  int max = tV*2;

  //distance
  int d[tV];
  //predecessor
  int p[tV];

  //on iinitialise le table de distance avec le max et les predecessor avec 0
  for (i = 0; i < tV; i++) {
    d[i] = max;
    p[i] = 0;
  }

  //on marque la source
  d[source] = 0;
  p[source] = 0;

  // on effectue tV-1 update pour trouver le chemin le plus cours
  for (i = 1; i <= tV - 1; i++) {
    for (j = 0; j < tE; j++) {
      // on récupère les donnée de chemin
      u = g->edge[j]->u;
      v = g->edge[j]->v;
      w = g->edge[j]->w;

      if (d[u] != max && d[v] > d[u] + w) {
        d[v] = d[u] + w;

        // si le cas ou le node + son poid dépasse le total des node alors il n'existe pas de prédécesseur
        if (d[u] + w > tV) {
          p[v] = -1;
        }else{
          p[v] = u;
        }

      }
    }
  }

  //On commence ici les modification
  // on cherche a avoir un tableau solution avec tout les prédescesseur jusqu'a la case origin
  int s[tV];
  s[0] = dest;
  int count = 1;

  //on remonte donc en prenent le prédécesseur de dest puis en attribuant le son prev a dest lui meme
  while (dest != source) {
    if (p[dest] == -1) {
      break;
    }
    s[count] = dest;
    dest = p[dest];
    count++;
  }
  s[count]= source;
  count++;

  //on traduit le tableau obtenu en une chaîne de caractère
  return translate_path(s,count);

}


// on traduit la chaine solution en solution
// on prend soin d'incrémenter countBellFord qui permet de si retrouver dans cette chaine de carac
void interpret_path(struct joueur *player, struct save *tab){
  int dest = player->currx+(player->curry-1)*tab->width;
  //on vérifie si on peut allez au nord
  if (path[countBellFord] == 'N' && (tab->data[dest]->value == '_' || tab->data[dest]->value == 'T')) {
    rep = "NORTH";
    countBellFord++;
  }else{
    dest = player->currx+1+player->curry*tab->width;
    //x+1 si a l'est
    if (path[countBellFord] == 'E' && (tab->data[dest]->value == '_' || tab->data[dest]->value == 'T')) {
      rep = "EAST";
      countBellFord++;
    }else{
      dest = player->currx-1+player->curry*tab->width;
      //x-1 si a l'ouest
      if (path[countBellFord] == 'W' && (tab->data[dest]->value == '_' || tab->data[dest]->value == 'T')) {
        rep = "WEST";
        countBellFord++;
      }else{
        //y+1 si au sud
        dest = player->currx+(player->curry+1)*tab->width;
        if (path[countBellFord] == 'S' && (tab->data[dest]->value == '_' || tab->data[dest]->value == 'T')) {
          rep = "SOUTH";
          countBellFord++;
          //sinon on est soit a la fin de la chaine soit on ne peut ce rendre aux prochain
        }else{
          rep = NULL;
          free(path);
          path = NULL;
          countBellFord = 0;
        }
      }
    }
  }
}


int main() {
  setbuf(stdout, NULL);
  char buf[BUFSIZE];

  struct joueur *player = malloc(sizeof(struct joueur));
  struct save *tab = malloc (sizeof(struct save));
  struct Graph *graph = malloc(sizeof(struct Graph));

  // get the width
  fgets(buf, BUFSIZE, stdin);
  tab->width = atoi(buf);
  // get the height
  fgets(buf, BUFSIZE, stdin);
  tab->height = atoi(buf);
  // get the initial x
  fgets(buf, BUFSIZE, stdin);
  player->posx_init = atoi(buf);
  player->currx = player->posx_init;
  // get the initial y
  fgets(buf, BUFSIZE, stdin);
  player->posy_init = atoi(buf);
  player->curry = player->posy_init;
  // get the treasure x
  fgets(buf, BUFSIZE, stdin);
  player->posx_treasure = atoi(buf);
  // get the treasure y
  fgets(buf, BUFSIZE, stdin);
  player->posy_treasure = atoi(buf);

  fprintf(stderr, "This will be put in error.log.\n");
  tab->data = calloc(tab->width*tab->height,sizeof(struct pave *));
  create_save(tab,player);
  fprintf(stderr, "height = %i \n", tab->height);
  fprintf(stderr, "width = %i \n", tab->width);
  print_distance_to_in_error(tab);



  // Allez sur le coffre
  for (int turn = 0; player->posx_treasure+player->posy_treasure*tab->width != player->currx+player->curry*tab->width; turn++) {
    // si on est déja sur le trésor alors autant rentrer a la case départ
    if (player->currx == player->posx_treasure && player->curry == player->posy_treasure) {
      break;
    }
    // update et initialisation
    fgets(buf, BUFSIZE, stdin);
    save_information(buf,tab,player);
    print_tab_in_error(tab);
    if (turn == 0) {
      create_graph(graph,tab);
    }

    //fprintf(stderr, "%s\n",path );
    if (path != NULL) {
      interpret_path(player,tab);
    }

    if (rep == NULL) {
      fprintf(stderr, "BELLMANN\n" );
      update(graph,tab,player,0);

      if (graph->nbNode != 0) {
        path = bellmanford(graph, player->currx+player->curry*tab->width,player->posx_treasure+player->posy_treasure*tab->width);
      }
      interpret_path(player,tab);
    }

    puts(rep);
    if (strcmp(rep,"NORTH") == 0) {
      player->curry--;
    }else{
      if (strcmp(rep,"EAST") == 0) {
        player->currx++;
      }else{
        if (strcmp(rep,"WEST") == 0) {
          player->currx--;
        }else{
          if (strcmp(rep,"SOUTH") == 0) {
            player->curry++;
          }
        }
      }
    }

    fprintf(stderr, "%i et %i \n",player->currx,player->curry );
    fprintf(stderr, "%s\n",rep );
    rep = NULL;

    // get the result
    fgets(buf, BUFSIZE, stdin);
    if (strcmp(buf, "END\n") == 0) {
      break;
    }
  }



  print_marked_to_in_error(tab);
  fprintf(stderr, "\n\n====================\n" );
  fprintf(stderr, "==ON A LE TRESOR !==\n" );
  fprintf(stderr, "====================\n" );
  fprintf(stderr, "\n\nBELLMANNFORD RETOUR !!!\n" );



  //retour a la case départ
  for (int turn = 0; player->currx+player->curry*tab->width != player->posx_init+player->posy_init*tab->width; turn++) {
    // update et initialisation
    fgets(buf, BUFSIZE, stdin);
    save_information(buf,tab,player);

    if (path != NULL) {
      interpret_path(player,tab);
    }

    if (turn == 0) {
      fprintf(stderr, "BELLMANN\n" );
      update(graph,tab,player,1);

      if (graph->nbNode != 0) {
        path = bellmanford(graph, player->currx+player->curry*tab->width,player->posx_init+player->posy_init*tab->width);
      }
      interpret_path(player,tab);
    }

    puts(rep);
    if (strcmp(rep,"NORTH") == 0) {
      player->curry--;
    }else{
      if (strcmp(rep,"EAST") == 0) {
        player->currx++;
      }else{
        if (strcmp(rep,"WEST") == 0) {
          player->currx--;
        }else{
          if (strcmp(rep,"SOUTH") == 0) {
            player->curry++;
          }
        }
      }
    }
    //fprintf(stderr, "%s\n",rep );
    rep = NULL;

    // get the result
    fgets(buf, BUFSIZE, stdin);
    if (strcmp(buf, "END\n") == 0) {
      break;
    }

  }

  destroy(tab,player,graph,path);
  return 0;
}
