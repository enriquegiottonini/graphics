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

struct Triangle* readTrianglesFromFile(const char* filename);
int linesInFile(const char* filename);
int facesInObj(const char* filename);
int verticesInObj(const char* filename);
void displayTri(void);
void (*getDisplayFunc(const char* filename))(void);
void endTri(void);

void matrixMultiplication(float A[4][4], float B[4][4], float C[4][4]);
struct Point matrixVectorMultiplication(float A[4][4], struct Point b);
void project_onto_xy(struct Triangle* triangles, int lines, int window);
void project_onto_yz(struct Triangle* triangles, int lines, int window);
void project_onto_xz(struct Triangle* triangles, int lines, int window);
void drawTriangle(struct Point a, struct Point b, struct Point c, int window);


#endif