#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#define BUFSIZE 256

struct pave {
  size_t marked;
  char value;
  size_t distance_to_treasure;
  size_t distance_to_best_point;
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

void destroy(struct save *tab,struct joueur *player){
  for (size_t i = 0; i < tab->width*tab->height; i++) {
    free(tab->data[i]);
  }
  free(tab->data);
  free(tab);
  free(player);
}

void save_information(char *buf,struct save *tab,struct joueur *player){
  fprintf(stderr, "%s\n",buf );
  int place = 0;
  for (int y = -1; y < 2; y++) {
    for (int x = -1; x < 2; x++) {
      if (y == 0 && x == 0) {
        tab->data[(player->currx)+(player->curry*tab->width)]->marked++;
        tab->data[(player->currx)+(player->curry*tab->width)]->value = 'P';
      }else{
        if (player->currx+x >= 0 && player->curry+y >= 0 && player->currx+x < tab->width && player->curry+y < tab->height) {
          tab->data[(player->currx+x) + (player->curry+y)*tab->width]->value =  buf[place];
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
      if (tab->data[j*tab->width+i]->marked < 10) {
        fprintf(stderr, "%li$|", tab->data[j*tab->width+i]->marked);
      }else{
        fprintf(stderr, "%li|", tab->data[j*tab->width+i]->marked);
      }

    }
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "\n");
  fprintf(stderr, "\n");
}

int find_best_pave(struct save *tab, struct joueur *player){
  int dist = tab->height*tab->width+1;
  int rep = player->currx+player->curry*tab->width;
  int dest;
  for (int y = 0; y < tab->height; y++) {
    for (int x = 0; x < tab->width; x++) {

      if (tab->data[y*tab->width+x]->marked > 0) {
        dest = (y-1)*tab->width+x;
        if (dest >= 0 && dest < tab->height*tab->width && y-1 > 0) {
          if (tab->data[dest]->value == '_' && tab->data[dest]->distance_to_treasure <= dist && tab->data[dest]->marked == 0) {
            rep = y*(tab->width)+x;
            dist = tab->data[dest]->distance_to_treasure;
          }
        }

        dest = y*tab->width+x+1;
        if (dest >= 0 && dest < tab->height*tab->width && x+1 < tab->width) {
          if (tab->data[dest]->value == '_' && tab->data[dest]->distance_to_treasure <= dist && tab->data[dest]->marked == 0) {
            rep = y*(tab->width)+x;
            dist = tab->data[dest]->distance_to_treasure;
          }
        }

        dest = y*(tab->width)+x-1;
        if (dest >= 0 && dest < tab->height*tab->width && x-1 > 0) {
          if (tab->data[dest]->value == '_' && tab->data[dest]->distance_to_treasure <= dist && tab->data[dest]->marked == 0) {
            rep = y*(tab->width)+x;
            dist = tab->data[dest]->distance_to_treasure;
          }
        }

        dest = (y+1)*tab->width+x;
        if (dest >= 0 && dest < tab->height*tab->width && y+1 < tab->height) {
          if (tab->data[dest]->value == '_' && tab->data[dest]->distance_to_treasure <= dist && tab->data[dest]->marked == 0) {
            rep = y*(tab->width)+x;
            dist = tab->data[dest]->distance_to_treasure;
          }
        }
      }
    }
  }

  return rep;
}


struct Edge *creat_edge(int origin, int dest, int weight){
  struct Edge *edge = malloc(sizeof(struct Edge));
  edge->u = origin;
  edge->v = dest;
  edge->w = weight;
  return edge;
}

int count_not_wall(struct save *tab){
  int count = 0;
  for (size_t i = 0; i < tab->height; i++) {
    for (size_t j = 0; j < tab->width; j++) {
      if (tab->data[i*tab->width+j]->value != '#') {
        count++;
      }
    }
  }
  return count;
}

int count_wall_to(struct save *tab,int dest){
  int count = 0;
  for (size_t i = 0; i < tab->height; i++) {
    for (size_t j = 0; j < tab->width; j++) {
      if (i*tab->width+j == dest) {
        return count;
      }
      if (tab->data[i*tab->width+j]->value == '#') {
        count++;
      }
    }
  }
  return count;
}

void destroy_node(struct Graph *graph){
  for (size_t i = 0; i < graph->nbEdge; i++) {
    free(graph->edge[i]);
  }
  free(graph->edge);
}


void create_graph(struct Graph *graph,struct save *tab){
  graph->nbNode = tab->width*tab->height;
  int max = (tab->width+10)*tab->height;
  //fprintf(stderr, "%i\n",graph->nbNode );
  graph->edge = calloc(graph->nbNode*4,sizeof(struct Edge *));
  int count_edge = 0;
  int dest,origin;
  for (int y = 0; y < tab->height; y++) {
    for (int x = 0; x < tab->width; x++) {

      origin = y*tab->width+x;
      if (tab->data[origin]->value == 'P' || tab->data[origin]->value == '_' || tab->data[origin]->value == '0') {

        dest = (y-1)*tab->width+x;
        if (dest >= 0 && y*tab->width >= dest) {
          if (tab->data[dest]->value == 'P' || tab->data[dest]->value == '_' || tab->data[dest]->value == '0') {
            graph->edge[count_edge] = creat_edge(origin,dest,1);
            //fprintf(stderr, "  %i to %i N with weight 1  |",origin ,graph->edge[count_edge]->v);
            count_edge++;
          }else{
            graph->edge[count_edge] = creat_edge(origin,dest,max);
            //fprintf(stderr, "  %i to %i N with weight max  |",origin ,graph->edge[count_edge]->v);
            count_edge++;
          }
        }

        dest = y*tab->width+x+1;
        if (dest < tab->height*tab->width && x+1 < tab->width) {
          if (tab->data[dest]->value == 'P' || tab->data[dest]->value == '_' || tab->data[dest]->value == '0') {
            graph->edge[count_edge] = creat_edge(origin,dest,1);
            //fprintf(stderr, "  %i to %i E with weight 1  |",origin ,graph->edge[count_edge]->v);
            count_edge++;
          }else{
            graph->edge[count_edge] = creat_edge(origin,dest,max);
            //fprintf(stderr, "  %i to %i E with weight max  |",origin ,graph->edge[count_edge]->v);
            count_edge++;
          }
        }

        dest = y*tab->width+x-1;
        if (dest >= 0 && x-1 >= 0) {
          if (tab->data[dest]->value == 'P' || tab->data[dest]->value == '_' || tab->data[dest]->value == '0') {
            graph->edge[count_edge] = creat_edge(origin,dest,1);
            //fprintf(stderr, "  %i to %i W with weight 1  |",origin ,graph->edge[count_edge]->v);
            count_edge++;
          }else{
            graph->edge[count_edge] = creat_edge(origin,dest,max);
            //fprintf(stderr, "  %i to %i W with weight max  |",origin ,graph->edge[count_edge]->v);
            count_edge++;
          }
        }

        dest = (y+1)*tab->width+x;
        if (dest < tab->height*tab->width && y+1 < tab->height) {
          if (tab->data[dest]->value == 'P' || tab->data[dest]->value == '_' || tab->data[dest]->value == '0') {
            graph->edge[count_edge] = creat_edge(origin,dest,1);
            //fprintf(stderr, "  %i to %i S with weight 1  |",origin ,graph->edge[count_edge]->v);
            count_edge++;
          }else{
            graph->edge[count_edge] = creat_edge(origin,dest,max);
            //fprintf(stderr, "  %i to %i S with weight max  |",origin ,graph->edge[count_edge]->v);
            count_edge++;
          }
        }
        //fprintf(stderr, "\n" );

      }else{
        graph->edge[count_edge] = creat_edge(origin,(y-1)*tab->width+x,max);
        count_edge++;
        graph->edge[count_edge] = creat_edge(origin,y*tab->width+x+1,max);
        count_edge++;
        graph->edge[count_edge] = creat_edge(origin,y*tab->width+x-1,max);
        count_edge++;
        graph->edge[count_edge] = creat_edge(origin,(y+1)*tab->width+x,max);
        count_edge++;
        //fprintf(stderr, "%i to NEWS with weight max\n",origin);
      }

    }
  }
  graph->nbEdge = count_edge;
  fprintf(stderr, "\n");
}


void FinalSolution(int dist[], int n) {
// This function prints the final solution
    fprintf(stderr, "\nVertex\tDistance from Source Vertex\n");
    int i;

    for (i = 0; i < n; ++i){
      fprintf(stderr, "%d \t\t %d\n", i, dist[i]);
    }
}


void display(int arr[], int size) {
  for (int i = 0; i < size; i++) {
    if (i%45 == 0) {
      fprintf(stderr, "\n");
    }
    if (arr[i] < 10) {
      fprintf(stderr, "0%d ", arr[i]);
    }else{
      fprintf(stderr, "%d ", arr[i]);
    }
  }
  fprintf(stderr, "\n");
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
  //fprintf(stderr, "%i\n",source );
  //fprintf(stderr, "%i\n",dest );
  //variables
  int i, j, u, v, w;

  //total vertex in the graph g
  int tV = g->nbNode;

  //total edge in the graph g
  int tE = g->nbEdge;
  int max = tV*2;

  //distance array
  //size equal to the number of vertices of the graph g
  int d[tV];

  //predecessor array
  //size equal to the number of vertices of the graph g
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

  //display(d,tV);
  //display(p, tV);
  //FinalSolution(d,tV);

  int s[tV];
  s[0] = dest;
  int count = 1;

  while (dest != source) {
    if (p[dest] == -1) {
      break;
    }
    s[count] = p[dest];
    dest = p[dest];
    fprintf(stderr, "%i ",dest );
    count++;
  }
  fprintf(stderr, "\n" );

  return translate_path(s,count);

}




int main() {
  setbuf(stdout, NULL);
  char buf[BUFSIZE];
  char *rep = '\0';

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

  int dist = tab->height*tab->width+1;
  int best_pave = 0;
  int countBellFord = 0;
  char *path = calloc(tab->height+tab->width,sizeof(char));
  path[0] = '\0';





  // initialize the state of the game
  for (int turn = 0; turn != 20; turn++) {
    // get the adjacent cells
    fgets(buf, BUFSIZE, stdin);
    save_information(buf,tab,player);
    print_tab_in_error(tab);

    /*
    best_pave = find_best_pave(tab,player);
    fprintf(stderr, "Best pave = %i\n",best_pave );
    if (best_pave == player->currx+player->curry*tab->width) {
      fprintf(stderr, "Best pave == Joueur \n" );
    }

    // update the state of the game
    // send the new direction
    // or "SOUTH" or "EAST" or "WEST"

    if (best_pave == player->currx+player->curry*tab->width) {
      dist = tab->height*tab->width+1;
      int dest = player->currx+(player->curry-1)*tab->width;
      if (buf[1] == '_' && tab->data[dest]->distance_to_treasure <= dist && tab->data[dest]->marked == 0) {
          rep = "NORTH";
          dist = tab->data[dest]->distance_to_treasure;
      }
      dest = player->currx+1+(player->curry)*tab->width;
      if (buf[4] == '_' && tab->data[dest]->distance_to_treasure <= dist && tab->data[dest]->marked == 0) {
          rep = "EAST";
          dist = tab->data[dest]->distance_to_treasure;
      }
      dest = player->currx-1+(player->curry)*tab->width;
      if (buf[3] == '_' && tab->data[dest]->distance_to_treasure <= dist && tab->data[dest]->marked == 0) {
        rep = "WEST";
        dist = tab->data[dest]->distance_to_treasure;
      }
      dest = player->currx+(player->curry+1)*tab->width;
      if (buf[6] == '_' && tab->data[dest]->distance_to_treasure <= dist && tab->data[dest]->marked == 0) {
        rep = "SOUTH";
        dist = tab->data[dest]->distance_to_treasure;
      }
      */
    //}else{
    fprintf(stderr, "test 1\n");
    if (path[0] != '\0') {
      fprintf(stderr, "test 2\n");
      if (path[countBellFord] == 'N') {
        rep = "NORTH";
        countBellFord++;
        fprintf(stderr, "test 2.1\n");
        }
      if (path[countBellFord] == 'E') {
        rep = "EAST";
        countBellFord++;
        fprintf(stderr, "test 2.2\n");
      }
      if (path[countBellFord] == 'W') {
        rep = "WEST";
        countBellFord++;
        fprintf(stderr, "test 2.3\n");
      }
      if (path[countBellFord] == 'S') {
        rep = "SOUTH";
        countBellFord++;
        fprintf(stderr, "test 2.4\n");
      }

    }else{
      fprintf(stderr, "test 3\n");
      countBellFord = 0;
      create_graph(graph,tab);
      fprintf(stderr, "test 4\n");

      if (graph->nbNode != 0) {
        fprintf(stderr, "test 5\n");
        fprintf(stderr, "%i\n",tab->width );
        fprintf(stderr, "%i et %i \n",player->currx,player->curry );
        path = bellmanford(graph, player->currx+player->curry*tab->width,player->posx_treasure+player->posy_treasure*tab->width);
      }

      destroy_node(graph);
    }

    //}



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

    // get the result
    fgets(buf, BUFSIZE, stdin);
    if (strcmp(buf, "END\n") == 0) {
      break;
    }
  }

  print_marked_to_in_error(tab);
  destroy(tab,player);
  return 0;
}
