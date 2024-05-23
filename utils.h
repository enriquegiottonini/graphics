#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include <math.h>

#define WIDTH 600
#define HEIGHT 600

struct Point
{
    float x;
    float y;
    float z;
};

struct Triangle
{
    struct Point a;
    struct Point b;
    struct Point c;
};

enum Window
{
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

enum Plane
{
    XY,
    YZ,
    XZ
};

struct Triangle* readTrianglesFromFile(const char* filename);
int linesInFile(const char* filename);
void displayTri(void);
void (*getDisplayFunc(const char* filename))(void);
void endTri(void);

void matrixMultiplication(float A[4][4], float B[4][4], float C[4][4]);
struct Point matrixVectorMultiplication(float A[4][4], struct Point b);
void project_onto_plane(struct Point* points, int N, int window, int plane, float scale);
void drawPoint(struct Point a, int window);
void copyMatrix(float A[4][4], float B[4][4]);
void readPointsFromObj(const char* filename);
int facesInObj(const char* filename);
int verticesInObj(const char* filename);
float getXMin(struct Point* points, int N);
float getXMax(struct Point* points, int N);
float getYMin(struct Point* points, int N);
float getYMax(struct Point* points, int N);
float getZMin(struct Point* points, int N);
float getZMax(struct Point* points, int N);
int readOnlyVertices(const char* filename);

#endif