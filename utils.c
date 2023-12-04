#include "utils.h"

struct Triangle* trianglesMemory;
struct Point* pointsMemory;
int trianglesCount;
int pointsCount;

void displayTri()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);

    if (trianglesMemory == NULL)
    {
        trianglesMemory = readTrianglesFromFile("OBJETOS-3D/itokawa_f0049152.tri");
        trianglesCount = linesInFile("OBJETOS-3D/itokawa_f0049152.tri");
    }

    if (pointsMemory == NULL && trianglesMemory != NULL)
    {
        pointsMemory = malloc(sizeof(struct Point) * trianglesCount * 3);
        for (int i=0; i<trianglesCount; i++)
        {
            struct Triangle triangle = trianglesMemory[i];
            pointsMemory[i*3] = triangle.a;
            pointsMemory[i*3 + 1] = triangle.b;
            pointsMemory[i*3 + 2] = triangle.c;
        }

        pointsCount = trianglesCount * 3;
    }

    project_onto_plane(pointsMemory, pointsCount, TOP_LEFT, XY);
    project_onto_plane(pointsMemory, pointsCount, TOP_RIGHT, YZ);
    project_onto_plane(pointsMemory, pointsCount, BOTTOM_LEFT, XZ);
    isometricProjection(pointsMemory, pointsCount, BOTTOM_RIGHT);

    glEnd();
	glutSwapBuffers();
}

float identityMatrix(float matrix[4][4])
{
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            matrix[i][j] = (i == j) ? 1 : 0;
}

float orthonormalMatrix(float A[4][4], int plane)
{
	float t[4][4] = {
		 {(plane==XZ?0:1), (plane==XZ?1:0),               0, 0},
		 {              0, (plane==XY?1:0), (plane==XY?0:1), 0},
		 {              0,               0,               0, 0},
		 {              0,               0,               0, 1}
	};

    float copyA[4][4];
    copyMatrix(A, copyA);

    matrixMultiplication(copyA, t, A);
}

void isometricMatrix(float A[4][4], float a, float b, float c){
    float I[4][4] = {
        {c/sqrt(a*a+c*c), 0, -a/sqrt(a*a+c*c), 0},
        {-a*b/sqrt((a*a+c*c)*(a*a+b*b+c*c)), sqrt((a*a+c*c)/(a*a+b*b+c*c)), -c*b/sqrt((a*a+c*c)*(a*a+b*b+c*c)), 0},
        {0,0,0,0},
        {0,0,0,1}
    };

    float copyA[4][4];
    copyMatrix(A, copyA);

    matrixMultiplication(copyA, I, A);
}

void getMinMaxInPlane(struct Point* points, int N, int plane, struct Point min, struct Point max)
{
    min.x = min.y = 1000000;
    max.x = max.y = -1000000;

    for (int i=0; i<N; i++)
    {
        struct Point point = points[i];
        switch (plane)
        {
            case XY:
                if (point.x < min.x) min.x = point.x;
                if (point.y < min.y) min.y = point.y;
                if (point.x > max.x) max.x = point.x;
                if (point.y > max.y) max.y = point.y;
                break;
            case YZ:
                if (point.y < min.x) min.x = point.y;
                if (point.z < min.y) min.y = point.z;
                if (point.y > max.x) max.x = point.y;
                if (point.z > max.y) max.y = point.z;
                break;
            case XZ:
                if (point.x < min.x) min.x = point.x;
                if (point.z < min.y) min.y = point.z;
                if (point.x > max.x) max.x = point.x;
                if (point.z > max.y) max.y = point.z;
                break;
        }
    }
}

void getViewportMatrix(struct Point min, struct Point max, int plane, float viewportMatrix[4][4])
{
    float dx, dy;
    float centerx = (max.x + min.x) / 2;
    float centery = (max.y + min.y) / 2;

    float sx = 1.0 / (max.x - min.x);
    float sy = 1.0 / (max.y - min.y);

    float T[4][4] = {
        {1, 0, 0, -centerx},
        {0, 1, 0, -centery},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    float S[4][4] = {
        {sx, 0, 0, 0},
        {0, sy, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    float T2[4][4] = {
        {1, 0, 0, centerx},
        {0, 1, 0, centery},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    float TS[4][4];
    matrixMultiplication(T, S, TS);

    float TST2[4][4];
    matrixMultiplication(TS, T2, TST2);

    float copyViewportMatrix[4][4];
    copyMatrix(viewportMatrix, copyViewportMatrix);
    matrixMultiplication(copyViewportMatrix, TST2, viewportMatrix);
}

void project_onto_plane(struct Point* points, int N, int window, int plane)
{
    float projectionMatrix[4][4];
    identityMatrix(projectionMatrix);

    struct Point min, max;
    getMinMaxInPlane(points, N, plane, min, max);
    //getViewportMatrix(min, max, plane, projectionMatrix);
    orthonormalMatrix(projectionMatrix, plane);

    for (int i=0; i<N; i++)
    {
        struct Point original = points[i];
        struct Point projected = matrixVectorMultiplication(projectionMatrix, original);
        drawPoint(projected, window);
    }
}

void isometricProjection(struct Point* points, int N, int window)
{
    float projectionMatrix[4][4];
    identityMatrix(projectionMatrix);
    isometricMatrix(projectionMatrix, 1, 1, 1);

    for (int i=0; i<N; i++)
    {
        struct Point original = points[i];
        struct Point projected = matrixVectorMultiplication(projectionMatrix, original);
        drawPoint(projected, window);
    }
}

void drawPoint(struct Point a, int window)
{
    float dx, dy;
    switch (window)
    {
        case TOP_LEFT:
            dx = -0.5;
            dy = 0.5;
            break;
        case TOP_RIGHT:
            dx = 0.5;
            dy = 0.5;
            break;
        case BOTTOM_LEFT:
            dx = -0.5;
            dy = -0.5;
            break;
        case BOTTOM_RIGHT:
            dx = 0.5;
            dy = -0.5;
            break;
    }

    glVertex3f(a.x + dx, a.y + dy, 0);
}

void matrixMultiplication(float A[4][4], float B[4][4], float C[4][4])
{
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            C[i][j] = A[i][0]*B[0][j] + A[i][1]*B[1][j] + A[i][2]*B[2][j] + A[i][3]*B[3][j];
}

void copyMatrix(float A[4][4], float B[4][4])
{
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            B[i][j] = A[i][j];
}



struct Point matrixVectorMultiplication(float A[4][4], struct Point b)
{
    struct Point c;
    c.x = A[0][0]*b.x + A[0][1]*b.y + A[0][2]*b.z + A[0][3];
    c.y = A[1][0]*b.x + A[1][1]*b.y + A[1][2]*b.z + A[1][3];
    c.z = A[2][0]*b.x + A[2][1]*b.y + A[2][2]*b.z + A[2][3];
    return c;
}

struct Triangle* readTrianglesFromFile(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Failed to open file: %s\n", filename);
        return;
    }

    if (strstr(filename, ".tri") != NULL)
    {
        char line[256];
        int lines = linesInFile(filename);

        struct Triangle* triangles = malloc(sizeof(struct Triangle) * lines);
        int counter = 0;
        while (fgets(line, sizeof(line), file))
        {
            struct Point a, b, c;
            int result = sscanf(line, "%f %f %f %f %f %f %f %f %f", &a.x, &a.y, &a.z, &b.x, &b.y, &b.z, &c.x, &c.y, &c.z);
            if (result == 9)
            {
                struct Triangle triangle = { a, b, c };
                triangles[counter] = triangle;
                counter++;
            }
        }
        fclose(file);
        return triangles;
    }

    printf("File format not supported: %s\n", filename);
    return NULL;
}


int linesInFile(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Failed to open file: %s\n", filename);
        return;
    }

    char line[256];
    int counter = 0;
    while (fgets(line, sizeof(line), file))
    {
        counter++;
    }
    fclose(file);
    return counter;
}

void (*getDisplayFunc(const char* filename))(void)
{
    if (strstr(filename, ".tri") != NULL)
    {
        return displayTri;
    }

    printf("File format not supported: %s\n", filename);
    return NULL;
}

void endTri()
{
    if (trianglesMemory != NULL)
        free(trianglesMemory);
}