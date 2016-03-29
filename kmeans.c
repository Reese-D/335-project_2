#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <float.h>

#define TOLERANCE 0.01f
#define LINE_SIZE 1000

typedef struct{
  float **points;
  float *centroid;
}cluster;

cluster *clusters;

float **AllPoints;
int Dimensions;

//------------------------------------------------------------------------------
// Function declaration
//------------------------------------------------------------------------------
void init(int k, int n);
void recalcCentroids(int k, int n);
double sse(int k, int n);
void assignClusters(int k, int n);
void cluster_(int k, int n);
int read();
double euclid(float *point1, float *point2);
void normalizeAndRemoveOutlier(int n);


/*******************************************************************************
* Find euclidean distance between two points
* TODO make sure Dimensions gets set when file read in
*******************************************************************************/
double euclid(float *point1, float *point2){
  float sum = 0;
  for(int x = 0; x < Dimensions; x++){
    sum += pow(point1[x] - point2[x], 2);
  }
  return sqrt(sum);
}


/*******************************************************************************
* reads in the csv file and allocates needed memory to store it
*******************************************************************************/
int read(){
  FILE *doc;
  int ctr = 0;
  Dimensions = 1;
  int lines = 1000;
  AllPoints = malloc(sizeof(float**) * lines);

  if ((doc = fopen("proj02data.csv", "r")) == NULL){
    printf("Couldn't open file");
    exit(1);
  }
  char currentLine[LINE_SIZE];
  fgets(currentLine,LINE_SIZE,doc); //skip first line


  //figure out how many Dimensions there are
  int myBool = 0;
  while(fgets(currentLine,LINE_SIZE,doc) != NULL){
    //find the number of Dimensions first time through
    if(!myBool){
      char copy[LINE_SIZE];
      strcpy(copy, currentLine);
      strtok(copy, ",");
      while(strtok(NULL, ",") != NULL) Dimensions++;
    }
    myBool = 1;

    //reallocate memory if there are more lines than expected
    if(ctr >= lines - 1){
      lines *= 2;
      AllPoints = realloc(AllPoints, sizeof(float**) * lines); //get more memory
    }
    AllPoints[ctr] = malloc(sizeof(float*) * Dimensions);
    AllPoints[ctr][0] = atof(strtok(currentLine, ","));
    #ifdef DEBUG
      printf("AllPoints: %f\t", AllPoints[ctr][0]);
    #endif
    for(int x = 1; x < Dimensions; x++){
      AllPoints[ctr][x] = atof(strtok(NULL, ","));
      #ifdef DEBUG
        printf("AllPoints: %f\t", AllPoints[ctr][x]);
      #endif
    }
    #ifdef DEBUG
      printf("\n");
    #endif
    ctr++;
  }
  AllPoints = realloc(AllPoints, sizeof(float**) * ctr); //shrink to exact value needed
  return ctr;
}


/*******************************************************************************
* the algorithm phase, pretty much ties everything together
*******************************************************************************/
void cluster_(int k, int n){
  clusters = malloc(sizeof(cluster) * k);
  for(int x = 0; x < k; x++){
    clusters[x].points = malloc(sizeof(float**) *n);
    clusters[x].centroid = malloc(sizeof(float*) * Dimensions);
  }
  normalizeAndRemoveOutlier(n);
  init(k, n);
  assignClusters(k, n);
  double errThis = sse(k, n);
  double errLast = errThis + 1;
  while(errLast - errThis > TOLERANCE){
    assignClusters(k,n);
    recalcCentroids(k,n);
    errLast = errThis;
    errThis = sse(k,n);
  }
  //TODO finish
}


/*******************************************************************************
* assigns each point to a cluster given its distance to the centroid
* leaves random NULL values in some of the cluster points that will have to be
* ignored later on
*******************************************************************************/
void assignClusters(int k, int n){
  for(int x = 0; x < k; x++){
    for(int y = 0; y < n; y++){
      clusters[x].points[y] = NULL; //clear out any former clusters
    }
  }
  for(int x = 0; x < n; x++){
    int y1 = -1;
    double lowestDistance = DBL_MAX;
    double currentDistance;

    for(int y = 0; y < k; y++){
      currentDistance = euclid(AllPoints[x], clusters[y].centroid);
      if(currentDistance < lowestDistance){
        lowestDistance = currentDistance;
        y1 = y;
      }
    }
    clusters[y1].points[x] = AllPoints[x];
  }
  #ifdef DEBUG_LOTS
    for(int x = 0; x < k; x++){
      for(int y = 0; y < n; y++){
        printf("cluster %i:\t", x);
        for(int z = 0; z < Dimensions; z++){
          if(clusters[x].points[y] != NULL) printf("%f\t",clusters[x].points[y][z]);
        }
        printf("\n");
      }
    }
  #endif
}


/*******************************************************************************
* randomly assigns centroids to given clusters
*******************************************************************************/
void init(int k, int n){
  srand(time(NULL));
  for(int x = 0; x < k; x++){
    for(int y = 0; y < Dimensions; y++){
      clusters[x].centroid[y] = AllPoints[rand() % n][y];
      #ifdef DEBUG
        printf("centroid: %f\t", clusters[x].centroid[y]);
      #endif
    }
    #ifdef DEBUG
      printf("\n");
    #endif
  }
}


/*******************************************************************************
* calculate sum squared error between cluster points and their centroid
*******************************************************************************/
double sse(int k, int n){
  double error = 0;
  for(int x = 0; x < n; x++){
    for(int y = 0; y < k; y++){
      if(clusters[y].points[x] != NULL){
        error += euclid(clusters[y].points[x], clusters[y].centroid);
      }
    }
  }
  error = pow(error,2);
  return error;
}


/*******************************************************************************
* re-calculate centroid points for clusters
*******************************************************************************/
void recalcCentroids(int k, int n){
  for(int x = 0; x < k; x++){ //for each cluster
    int ctr = 0;
    float *temp = calloc(Dimensions, sizeof(float*));
    for(int y = 0; y < n; y++){ //for each point
      if(clusters[x].points[y] != NULL){
        ctr++; //count the number of points in the cluster
        for(int z = 0;z < Dimensions; z++){ //for each variable
          temp[z] += clusters[x].points[y][z]; //add up all the variables
        }
      }
    }
    printf("Recalculated centroid for cluster %i:",x);
    for(int z = 0; z < Dimensions; z++){
      if(ctr != 0) clusters[x].centroid[z] = temp[z]/(float)ctr; //find the mean
      #ifdef DEBUG
        printf("%f\t",clusters[x].centroid[z]);
      #endif
    }
    #ifdef DEBUG
      printf("\n");
    #endif
  }
}


/*******************************************************************************
*   Normalize the data and removes outliers in data set
*   Uses core normalization so values don't lie perfect between 0-1, the majority
*   will lie between these values but a few may lie outside them
*******************************************************************************/
void normalizeAndRemoveOutlier(int n){
  float *temp = calloc(Dimensions, sizeof(float*));
  float *max = calloc(Dimensions, sizeof(float*));
  float *min = calloc(Dimensions, sizeof(float*));
  for(int x = 0; x < Dimensions; x++){
    for(int y = 0; y < n; y++){
      if(AllPoints[y][x] > max[x]) max[x] = AllPoints[y][x]; //find min value
      if(AllPoints[y][x] < min[x]) min[x] = AllPoints[y][x]; //find max value
      temp[x] += AllPoints[y][x]; //add up all values
    }
    temp[x] /= (float) n; //find the mean
  }

  for(int x = 0; x < Dimensions; x++){
    for(int y = 0; y < n; y++){
      if(AllPoints[y][x] > temp[x]*2){
        AllPoints[y][x] = temp[x]; //remove outlier by making it the mean
      }else if(AllPoints[y][x] < temp[x]/5){
        AllPoints[y][x] = temp[x]; //remove low outliers
      }
      AllPoints[y][x] = (AllPoints[y][x] - min[x]) / (max[x] - min[x]); // normalize
    }
  }

  #ifdef DEBUG
    for(int x = 0; x < n; x++){
        printf("All Points %i:", x);
      for(int y = 0; y < Dimensions; y++){
          printf(" %f\t", AllPoints[x][y]);
      }
        printf("\n");
    }
  #endif
}





/*******************************************************************************
* main function
*******************************************************************************/
int main(int argc, char *argv[]){
  int n = read();
  cluster_(4, n);
  return 0;
}
