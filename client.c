#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#define BUFSIZE 256

//on déclare quelque variable global pour pouvoir les modifier simplement dans une fonction de type void
char *rep = '\0';
int countBellFord = 0;
char *path = NULL;

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

struct pave *create_pave(){
  struct pave *curr = malloc(sizeof(struct pave));
  curr->marked = 0;
  curr->value = '0';
  return curr;
}

void create_save(struct save *self, struct joueur *player){
  for (size_t i = 0; i < self->height; i++) {
    for (size_t j = 0; j < self->width; j++) {
      self->data[i*self->width+j] = create_pave();
      self->data[i*self->width+j]->distance_to_treasure = sqrt((player->posx_treasure-j)*(player->posx_treasure-j) + (player->posy_treasure-i)*(player->posy_treasure-i));
    }
  }
}

void destroy_node(struct Graph *graph){
  for (size_t i = 0; i < graph->nbEdge; i++) {
    free(graph->edge[i]);
  }
  free(graph->edge);
}

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

void save_information(char *buf,struct save *tab,struct joueur *player){
  //fprintf(stderr, "%s\n",buf );
  int place = 0;
  for (int y = -1; y < 2; y++) {
    for (int x = -1; x < 2; x++) {
      if (y == 0 && x == 0) {
        tab->data[player->currx+(player->curry*tab->width)]->marked++;
        tab->data[player->currx+(player->curry*tab->width)]->value = 'P';
      }else{
        if (player->currx+x >= 0 && player->curry+y >= 0 && player->currx+x < tab->width && player->curry+y < tab->height) {
          tab->data[(player->currx+x) + (player->curry+y)*tab->width]->value = buf[place];
        }
        place++;
      }
    }
  }
}

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


struct Edge *creat_edge(int origin, int dest, int weight){
  struct Edge *edge = malloc(sizeof(struct Edge));
  edge->u = origin;
  edge->v = dest;
  edge->w = weight;
  return edge;
}



void creat_graph_partial(struct Graph *graph,struct save *tab, int origin, int dest, int count_edge, int max){
  if (tab->data[dest]->value == 'P' || tab->data[dest]->value == '_' || tab->data[dest]->value == '0') {
    graph->edge[count_edge] = creat_edge(origin,dest,1);
  }else{
    graph->edge[count_edge] = creat_edge(origin,dest,max);
  }
}


void create_graph(struct Graph *graph,struct save *tab){
  graph->nbNode = tab->width*tab->height;
  graph->edge = calloc(graph->nbNode*4,sizeof(struct Edge *));

  int max = graph->nbNode*2;
  int count_edge = 0;
  int dest,origin;

  for (int y = 0; y < tab->height; y++) {
    for (int x = 0; x < tab->width; x++) {

      origin = y*tab->width+x;

      if (tab->data[origin]->value == 'P' || tab->data[origin]->value == '_' || tab->data[origin]->value == '0') {
        dest = (y-1)*tab->width+x;
        if (dest >= 0 && y*tab->width >= dest) {
          creat_graph_partial(graph,tab,origin,dest,count_edge,max);
          count_edge++;
        }else{
          graph->edge[count_edge] = creat_edge(origin,origin,max);
          count_edge++;
        }

        dest = y*tab->width+x+1;
        if (dest < tab->height*tab->width && x+1 < tab->width) {
          creat_graph_partial(graph,tab,origin,dest,count_edge,max);
          count_edge++;
        }else{
          graph->edge[count_edge] = creat_edge(origin,origin,max);
          count_edge++;
        }

        dest = y*tab->width+x-1;
        if (dest >= 0 && x-1 >= 0) {
          creat_graph_partial(graph,tab,origin,dest,count_edge,max);
          count_edge++;
        }else{
          graph->edge[count_edge] = creat_edge(origin,origin,max);
          count_edge++;
        }

        dest = (y+1)*tab->width+x;
        if (dest < tab->height*tab->width && y+1 < tab->height) {
          creat_graph_partial(graph,tab,origin,dest,count_edge,max);
          count_edge++;
        }else{
          graph->edge[count_edge] = creat_edge(origin,origin,max);
          count_edge++;
        }

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
  graph->nbEdge = count_edge;
  fprintf(stderr, "\n");
}


void update_partial(struct Graph *graph,struct save *tab, int origin, int dest, int count_edge,int min, int max){
  if (tab->data[dest]->value == 'P' || tab->data[dest]->value == '_' || tab->data[dest]->value == '0' || tab->data[dest]->value == 'T') {
    graph->edge[count_edge]->w = min;
  }else{
    graph->edge[count_edge]->w = max;
  }
}

void update(struct Graph *graph,struct save *tab,struct joueur *player, int onlymarked){
  int count_edge = 0;
  int max = graph->nbNode*2;
  int dest,origin;
  int disty = tab->height-abs(player->posy_treasure-player->curry);
  int distx = tab->width-abs(player->posx_treasure-player->currx);
  for (int y = 0; y < tab->height; y++) {
    for (int x = 0; x < tab->width; x++) {

      origin = y*tab->width+x;
      if (onlymarked == 1) {
        if (tab->data[origin]->marked > 0 && (tab->data[origin]->value == 'P' || tab->data[origin]->value == '_' || tab->data[origin]->value == 'T')) {

                  dest = (y-1)*tab->width+x;
                  if (dest >= 0 && y*tab->width >= dest) {
                    update_partial(graph,tab,origin,dest,count_edge,1,max);
                    count_edge++;
                  }else{
                    update_partial(graph,tab,origin,origin,count_edge,max,max);
                    count_edge++;
                  }

                  dest = y*tab->width+x+1;
                  if (dest < tab->height*tab->width && x+1 < tab->width) {
                    update_partial(graph,tab,origin,dest,count_edge,1,max);
                    count_edge++;
                  }else{
                    update_partial(graph,tab,origin,origin,count_edge,max,max);
                    count_edge++;
                  }

                  dest = y*tab->width+x-1;
                  if (dest >= 0 && x-1 >= 0) {
                    update_partial(graph,tab,origin,dest,count_edge,1,max);
                    count_edge++;
                  }else{
                    update_partial(graph,tab,origin,origin,count_edge,max,max);
                    count_edge++;
                  }

                  dest = (y+1)*tab->width+x;
                  if (dest < tab->height*tab->width && y+1 < tab->height) {
                    update_partial(graph,tab,origin,dest,count_edge,1,max);
                    count_edge++;
                  }else{
                    update_partial(graph,tab,origin,origin,count_edge,max,max);
                    count_edge++;
                  }

        }else{
          for (size_t i = 0; i < 4; i++) {
            graph->edge[count_edge]->w = max;
            count_edge++;
          }
        }
      }else{
        if (tab->data[origin]->value == 'P' || tab->data[origin]->value == '_' || tab->data[origin]->value == '0' || tab->data[origin]->value == 'T') {

          dest = (y-1)*tab->width+x;
          if (dest >= 0 && y*tab->width >= dest) {
            update_partial(graph,tab,origin,dest,count_edge,1,max);
            count_edge++;
          }else{
            update_partial(graph,tab,origin,origin,count_edge,max,max);
            count_edge++;
          }

          dest = y*tab->width+x+1;
          if (dest < tab->height*tab->width && x+1 < tab->width) {
            update_partial(graph,tab,origin,dest,count_edge,1,max);
            count_edge++;
          }else{
            update_partial(graph,tab,origin,origin,count_edge,max,max);
            count_edge++;
          }

          dest = y*tab->width+x-1;
          if (dest >= 0 && x-1 >= 0) {
            update_partial(graph,tab,origin,dest,count_edge,1,max);
            count_edge++;
          }else{
            update_partial(graph,tab,origin,origin,count_edge,max,max);
            count_edge++;
          }

          dest = (y+1)*tab->width+x;
          if (dest < tab->height*tab->width && y+1 < tab->height) {
            update_partial(graph,tab,origin,dest,count_edge,1,max);
            count_edge++;
          }else{
            update_partial(graph,tab,origin,origin,count_edge,max,max);
            count_edge++;
          }

        }else{
          for (size_t i = 0; i < 4; i++) {
            graph->edge[count_edge]->w = max;
            count_edge++;
          }
        }
      }


    }
  }
  fprintf(stderr, "\n");
}



char *translate_path(int s[], int count){
  char *path = calloc(count,sizeof(char));
  int lenght = 0;
  if (count <= 1) {
    path[0] = '\0';
    return path;
  }

  for (size_t i = count-2; i != 0; i--) {
    if (s[i+1] == s[i]+1) {
      path[lenght] = 'W';
      //fprintf(stderr, "E\n");
      lenght++;
    }else{
      if (s[i+1] == s[i]-1) {
        path[lenght] = 'E';
        //fprintf(stderr, "W\n");
        lenght++;
      }else{
        if (s[i+1] > s[i]) {
          path[lenght] = 'N';
          //fprintf(stderr, "S\n");
          lenght++;
        }else{
          if (s[i+1] < s[i]) {
            path[lenght] = 'S';
            //fprintf(stderr, "N\n");
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

char *bellmanford(struct Graph *g, int source, int dest) {

  int i, j, u, v, w;
  int tV = g->nbNode;
  int tE = g->nbEdge;
  int max = tV*2;

  //distance array
  int d[tV];
  //predecessor array
  int p[tV];

  //step 1: fill the distance array and predecessor array
  for (i = 0; i < tV; i++) {
    d[i] = max;
    p[i] = 0;
  }

  //mark the source vertex
  d[source] = 0;
  p[source] = 0;

  //step 2: relax edges |V| - 1 times
  for (i = 1; i <= tV - 1; i++) {
    for (j = 0; j < tE; j++) {
      //get the edge data
      u = g->edge[j]->u;
      v = g->edge[j]->v;
      w = g->edge[j]->w;

      if (d[u] != max && d[v] > d[u] + w) {
        d[v] = d[u] + w;
        if (d[u] + w > tV) {
          p[v] = -1;
        }else{
          p[v] = u;
        }

      }
    }
  }

  int s[tV];
  s[0] = dest;
  int count = 1;

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

  return translate_path(s,count);

}



void interpret_path(struct joueur *player, struct save *tab){
  int dest = player->currx+(player->curry-1)*tab->width;
  if (path[countBellFord] == 'N' && (tab->data[dest]->value == '_' || tab->data[dest]->value == 'T')) {
    //fprintf(stderr, "On continue le chemin\n" );
    rep = "NORTH";
    countBellFord++;
  }else{
    dest = player->currx+1+player->curry*tab->width;
    if (path[countBellFord] == 'E' && (tab->data[dest]->value == '_' || tab->data[dest]->value == 'T')) {
      //fprintf(stderr, "On continue le chemin\n" );
      rep = "EAST";
      countBellFord++;
    }else{
      dest = player->currx-1+player->curry*tab->width;
      if (path[countBellFord] == 'W' && (tab->data[dest]->value == '_' || tab->data[dest]->value == 'T')) {
        //fprintf(stderr, "On continue le chemin\n" );
        rep = "WEST";
        countBellFord++;
      }else{
        dest = player->currx+(player->curry+1)*tab->width;
        if (path[countBellFord] == 'S' && (tab->data[dest]->value == '_' || tab->data[dest]->value == 'T')) {
          //fprintf(stderr, "On continue le chemin\n" );
          rep = "SOUTH";
          countBellFord++;
        }else{
          //fprintf(stderr, "On continue PAS le chemin\n" );
          rep = NULL;
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

  int dist,prev_dist,dest;


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
    if (countBellFord != 0) {
      interpret_path(player,tab);
    }

    if (rep == NULL) {
      /*
      fprintf(stderr, "Le plus proche\n" );
      dist = tab->height*tab->width+1;
      fprintf(stderr, "%i largeur et %i longueur\n",abs(player->posy_treasure - player->curry), abs(player->posx_treasure - player->currx ));


      if (abs(player->posx_treasure - player->currx) > abs(player->posy_treasure - player->curry)) {
        if (player->posx_treasure - player->currx < 0) {
          dest = player->currx-1+player->curry*tab->width;
          if ((buf[3] == '_' || buf[3] == 'T') && tab->data[dest]->distance_to_treasure <= dist && tab->data[dest]->marked == 0) {
            rep = "WEST";
            dist = tab->data[dest]->distance_to_treasure;
          }
        }
        if (player->posx_treasure - player->currx > 0) {
          dest = player->currx+1+player->curry*tab->width;
          if ((buf[4] == '_' || buf[4] == 'T') && tab->data[dest]->distance_to_treasure <= dist && tab->data[dest]->marked == 0) {
            rep = "EAST";
            dist = tab->data[dest]->distance_to_treasure;
          }
        }
      }

      if (abs(player->posy_treasure - player->curry) >  abs(player->posx_treasure - player->currx )) {
        if (player->posy_treasure - player->curry < 0) {
          dest = player->currx+(player->curry-1)*tab->width;
          if ((buf[1] == '_' || buf[1] == 'T') && tab->data[dest]->distance_to_treasure <= dist && tab->data[dest]->marked == 0) {
            rep = "NORTH";
            dist = tab->data[dest]->distance_to_treasure;
          }
        }
        if (player->posy_treasure - player->curry > 0) {
          dest = player->currx+(player->curry+1)*tab->width;
          if ((buf[6] == '_' || buf[6] == 'T') && tab->data[dest]->distance_to_treasure <= dist && tab->data[dest]->marked == 0) {
            rep = "SOUTH";
            dist = tab->data[dest]->distance_to_treasure;
          }
        }
      }



      if (rep == NULL) {
        dest = player->currx+(player->curry-1)*tab->width;
        if ((buf[1] == '_' || buf[1] == 'T') && tab->data[dest]->distance_to_treasure <= dist && tab->data[dest]->marked == 0) {
          rep = "NORTH";
          dist = tab->data[dest]->distance_to_treasure;
        }
        dest = player->currx+1+player->curry*tab->width;
        if ((buf[4] == '_' || buf[4] == 'T') && tab->data[dest]->distance_to_treasure <= dist && tab->data[dest]->marked == 0) {
          rep = "EAST";
          dist = tab->data[dest]->distance_to_treasure;
        }
        dest = player->currx-1+player->curry*tab->width;
        if ((buf[3] == '_' || buf[3] == 'T') && tab->data[dest]->distance_to_treasure <= dist && tab->data[dest]->marked == 0) {
          rep = "WEST";
          dist = tab->data[dest]->distance_to_treasure;
        }
        dest = player->currx+(player->curry+1)*tab->width;
        if ((buf[6] == '_' || buf[6] == 'T') && tab->data[dest]->distance_to_treasure <= dist && tab->data[dest]->marked == 0) {
          rep = "SOUTH";
          dist = tab->data[dest]->distance_to_treasure;
        }
      }

      if (dist > prev_dist) {
        rep = NULL;
      }
      prev_dist = dist;
      */

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
