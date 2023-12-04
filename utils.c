#include "utils.h"

struct Triangle* trianglesMemory;
int trianglesCount;

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

    project_onto_xy(trianglesMemory, trianglesCount, 1);
    project_onto_xz(trianglesMemory, trianglesCount, 2);
    project_onto_yz(trianglesMemory, trianglesCount, 3);
    proyect_isometric(trianglesMemory, trianglesCount, 4);

    glEnd();
	glutSwapBuffers();
}
void displayObj(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);

    if (trianglesMemory == NULL)
    {
        trianglesMemory = readTrianglesFromFile("OBJETOS-3D/QueSoy2.obj");
        trianglesCount = facesInObj("OBJETOS-3D/QueSoy2.obj");
    }

    project_onto_xy(trianglesMemory, trianglesCount, 5);
    project_onto_xz(trianglesMemory, trianglesCount, 6);
    project_onto_yz(trianglesMemory, trianglesCount, 7);
    proyect_isometric(trianglesMemory, trianglesCount, 8);

    glEnd();
	glutSwapBuffers();
}
float identityMatrix(float matrix[4][4])
{
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            matrix[i][j] = (i == j) ? 1 : 0;
}

float orthonormalMatrix(float A[4][4], int plano)
{
    float t[16] = {
        (plano == 2 ? 0 : 1), (plano == 2 ? 1 : 0), 0, 0,
        0, (plano == 0 ? 1 : 0), (plano == 0 ? 0 : 1), 0,
        0, 0, 0, 0,
        0, 0, 0, 1
    };

    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            A[i][j] = t[i*4 + j];
}

void isometricMatrix(float A[4][4], float a, float b, float c){
	float I[16] = {
		c/sqrt(a*a+c*c), 0, -a/sqrt(a*a+c*c), 0,
		-a*b/sqrt((a*a+c*c)*(a*a+b*b+c*c)), sqrt((a*a+c*c)/(a*a+b*b+c*c)), -c*b/sqrt((a*a+c*c)*(a*a+b*b+c*c)), 0,
		0,0,0,0,
		0,0,0,1
	};
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            A[i][j] = I[i*4 + j];
}

void project_onto_xy(struct Triangle* triangles, int lines, int window)
{
    float projectionMatrix[4][4];
    identityMatrix(projectionMatrix);
    orthonormalMatrix(projectionMatrix, 0);

    struct Point min, max;


    for (int i=0; i<lines; i++)
    {
        struct Triangle triangle = triangles[i];
        struct Point a = matrixVectorMultiplication(projectionMatrix, triangle.a);
        struct Point b = matrixVectorMultiplication(projectionMatrix, triangle.b);
        struct Point c = matrixVectorMultiplication(projectionMatrix, triangle.c);
        drawTriangle(a, b, c, window);
    }
}


void project_onto_xz(struct Triangle* triangles, int lines, int window)
{
    float projectionMatrix[4][4];
    identityMatrix(projectionMatrix);
    orthonormalMatrix(projectionMatrix, 2);

    for (int i=0; i<lines; i++)
    {
        struct Triangle triangle = triangles[i];
        struct Point a = matrixVectorMultiplication(projectionMatrix, triangle.a);
        struct Point b = matrixVectorMultiplication(projectionMatrix, triangle.b);
        struct Point c = matrixVectorMultiplication(projectionMatrix, triangle.c);
        drawTriangle(a, b, c, window);
    }
}

void project_onto_yz(struct Triangle* triangles, int lines, int window)
{
    float projectionMatrix[4][4];
    identityMatrix(projectionMatrix);
    orthonormalMatrix(projectionMatrix, 1);

    for (int i=0; i<lines; i++)
    {
        struct Triangle triangle = triangles[i];
        struct Point a = matrixVectorMultiplication(projectionMatrix, triangle.a);
        struct Point b = matrixVectorMultiplication(projectionMatrix, triangle.b);
        struct Point c = matrixVectorMultiplication(projectionMatrix, triangle.c);
        drawTriangle(a, b, c, window);
    }
}


void proyect_isometric(struct Triangle* triangles, int lines, int window)
{
    float projectionMatrix[4][4];
    identityMatrix(projectionMatrix);
    isometricMatrix(projectionMatrix, 1, 1, 1);

    for (int i=0; i<lines; i++)
    {
        struct Triangle triangle = triangles[i];
        struct Point a = matrixVectorMultiplication(projectionMatrix, triangle.a);
        struct Point b = matrixVectorMultiplication(projectionMatrix, triangle.b);
        struct Point c = matrixVectorMultiplication(projectionMatrix, triangle.c);
        drawTriangle(a, b, c, window);
    }
}


void drawTriangle(struct Point a, struct Point b, struct Point c, int window)
{
    float dx, dy;
    switch (window)
    {
        case 1:
            dx = -0.5;
            dy = 0.5;
            break;
        case 2:
            dx = 0.5;
            dy = 0.5;
            break;
        case 3:
            dx = -0.5;
            dy = -0.5;
            break;
        case 4:
            dx = 0.5;
            dy = -0.5;
            break;
        case 5:
            dx = -1;
            dy = 0;
            break;
        case 6:
            dx=-0.1;
            break;
        case 7:
            dx=-1;
            dy=-1;
            break;
        case 8:
            dx=0.5;
            dy=-0.5;
            break;

    }

    glVertex3f(a.x + dx, a.y + dy, 0);
    glVertex3f(b.x + dx, b.y + dy, 0);
    glVertex3f(c.x + dx, c.y + dy, 0);

}

void matrixMultiplication(float A[4][4], float B[4][4], float C[4][4])
{
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            C[i][j] = A[i][0]*B[0][j] + A[i][1]*B[1][j] + A[i][2]*B[2][j] + A[i][3]*B[3][j];
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
    } else if (strstr(filename, ".obj")!=NULL){
        char line[256];
        int vertices = verticesInObj(filename);
        int faces = facesInObj(filename);
        printf("Vertices: %d, faces: %d", vertices, faces);
        struct Triangle* triangles = malloc(sizeof(struct Triangle) * faces);
        struct Point* points = malloc(sizeof(struct Point)*vertices);
        int npoints = 0;
        int ntriangles = 0;
        while (fgets(line, sizeof(line), file)) {
        //printf("Line: %s", line);
        if (line[0] == 'v' && line[1] == ' ') {
            int result = sscanf(line + 2, "%f %f %f", &points[npoints].x, &points[npoints].y, &points[npoints].z);
            npoints++;
        } else if (line[0] == 'f' && line[1] == ' ') {
            int p1, p2, p3;
            int result = sscanf(line + 2, "%d %d %d", &p1, &p2, &p3);
            p1--; p2--; p3--;
            struct Triangle triangle = {points[p1],points[p2],points[p3]};
            triangles[ntriangles]=triangle;
            ntriangles++;
        } else {
            printf("Skipping line: %s", line);
        }
        }   
        free(points);
        return triangles;
    }

    printf("File format not supported: %s\n", filename);
    return NULL;
}
int facesInObj(const char* filename){
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
        if(line[0]=='f'){
            counter++;
        }
    }
    fclose(file);
    return counter;
}
int verticesInObj(const char* filename){
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
        if(line[0]=='v'){
            counter++;
        }
    }
    fclose(file);
    return counter;
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
    if (strstr(filename, ".obj") != NULL)
    {
        return displayObj;
    }

    printf("File format not supported: %s\n", filename);
    return NULL;
}

void endTri()
{
    if (trianglesMemory != NULL)
        free(trianglesMemory);
}