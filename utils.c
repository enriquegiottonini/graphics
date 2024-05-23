#include "utils.h"

struct Triangle* trianglesMemory;
struct Point* pointsMemory;
int trianglesCount;
int pointsCount;
const char* filenameOk = "ola";

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

    project_onto_plane(pointsMemory, pointsCount, TOP_LEFT, XY, 1.5);
    project_onto_plane(pointsMemory, pointsCount, TOP_RIGHT, YZ, 2.5);
    project_onto_plane(pointsMemory, pointsCount, BOTTOM_LEFT, XZ, 1.5);
    isometricProjection(pointsMemory, pointsCount, BOTTOM_RIGHT, 1);

    glEnd();
	glutSwapBuffers();
}

void displayObj()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);

    if (pointsMemory == NULL) {
        readPointsFromObj(filenameOk);
    }

    if (strcmp(filenameOk, "OBJETOS-3D/QueSoy1.obj") == 0){
        project_onto_plane(pointsMemory, pointsCount, 4, XY, 1.5);
        project_onto_plane(pointsMemory, pointsCount, 5, YZ, 1.5);
        project_onto_plane(pointsMemory, pointsCount, 6, XZ, 1.5);
        isometricProjection(pointsMemory, pointsCount, 7, 1);
    } else {
        project_onto_plane(pointsMemory, pointsCount, 8, XY, 1);
        project_onto_plane(pointsMemory, pointsCount, 9, YZ, 1);
        project_onto_plane(pointsMemory, pointsCount, 10, XZ, 1);
        isometricProjection(pointsMemory, pointsCount, 11, 1);
    }

    glEnd();
    glutSwapBuffers();
}

void displayEros()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);

    if (pointsMemory == NULL) {
        readPointsFromObj("OBJETOS-3D/eros022540.tab");
    }
    project_onto_plane(pointsMemory, pointsCount, TOP_LEFT, XY, 1.6);
    project_onto_plane(pointsMemory, pointsCount, 13, YZ, 2.3);
    project_onto_plane(pointsMemory, pointsCount, 14, XZ, 1.3);
    isometricProjection(pointsMemory, pointsCount, 15, 1);

    glEnd();
    glutSwapBuffers();

}

void displayGeo()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);

    if (pointsMemory == NULL) {
        readPointsFromObj("OBJETOS-3D/1620geographos.tab");
    }

    printf("%d\n", pointsCount);

    project_onto_plane(pointsMemory, pointsCount, 12, XY, 1.5);
    project_onto_plane(pointsMemory, pointsCount, 13, YZ, 2.5);
    project_onto_plane(pointsMemory, pointsCount, 14, XZ, 1.5);
    isometricProjection(pointsMemory, pointsCount, 15, 1);

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

void getViewportMatrix(float mina, float maxa, float minb, float maxb, int window, int plane, float matrix[4][4], float scale)
{
    float tx = -mina;
    float ty = -minb;
    float sx = 1 / (scale * (maxa - mina));
    float sy = 1 /  (scale * (maxb - minb));

    float matrix1[4][4] = {
        {sx, 0, 0, 0},
        {0, sy, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    float matrixCopy[4][4];
    copyMatrix(matrix, matrixCopy);
    matrixMultiplication(matrix1, matrixCopy, matrix);
}

void project_onto_plane(struct Point* points, int N, int window, int plane, float scale)
{
    float projectionMatrix[4][4];
    identityMatrix(projectionMatrix);
    float mina, maxa, minb, maxb;
    switch (plane)
    {
        case XY:
            mina = getXMin(points, N);
            maxa = getXMax(points, N);
            minb = getYMin(points, N);
            maxb = getYMax(points, N);
            break;
        case YZ:
            mina = getYMin(points, N);
            maxa = getYMax(points, N);
            minb = getZMin(points, N);
            maxb = getZMax(points, N);
            break;
        case XZ:
            mina = getXMin(points, N);
            maxa = getXMax(points, N);
            minb = getZMin(points, N);
            maxb = getZMax(points, N);
            break;
    }
    if (strstr(filenameOk, "QueSoy2.obj") == 0)
        getViewportMatrix(mina, maxa, minb, maxb, window, plane, projectionMatrix, scale);
    orthonormalMatrix(projectionMatrix, plane);

    for (int i=0; i<N; i++)
    {
        struct Point original = points[i];
        struct Point projected = matrixVectorMultiplication(projectionMatrix, original);
        drawPoint(projected, window);
    }
}

void isometricProjection(struct Point* points, int N, int window, float scale)
{
    float projectionMatrix[4][4];
    identityMatrix(projectionMatrix);
    isometricMatrix(projectionMatrix, 1, 1, 1);
    float mina, maxa, minb, maxb;
    mina = getXMin(points, N);
    maxa = getXMax(points, N);
    minb = getYMin(points, N);
    maxb = getYMax(points, N);
    if (strstr(filenameOk, "QueSoy2.obj") == 0)
        getViewportMatrix(mina, maxa, minb, maxb, window, XY, projectionMatrix, 1.5);

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
        case 4:
            dx = -0.4;
            dy = 0.4;
            break;
        case 5:
            dx = 0.4;
            dy = 0.8;
            break;
        case 6:
            dx = -0.4;
            dy = -0;
            break;
        case 7:
            dx = 0.2;
            dy = -0.6;
            break;
        case 8:
            dx = -1;
            dy = 0;
            break;
        case 9:
            dx=-0.1;
            break;
        case 10:
            dx=-1;
            dy=-1;
            break;
        case 11:
            dx=0.5;
            dy=-0.5;
            break;
        case 12:
            dx=-0.5;
            dy=0.5;
            break;
        case 13:
            dx=0.4;
            dy=0.5;
            break;
        case 14:
            dx=-0.5;
            dy=-0.5;
            break;
        case 15:
            dx=0.5;
            dy=-0.5;
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

void readPointsFromObj(const char* filename){
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Failed to open file: %s\n", filename);
        return;
    }
    char line[256];
    int vertices = verticesInObj(filename);
    int faces = facesInObj(filename);
    pointsMemory = malloc(sizeof(struct Point) * (vertices + faces*3));
    pointsCount = 0;

    while (fgets(line, sizeof(line), file))
    {
        if(line[0]=='#'){
            continue;
        }
        if (line[0]=='f'){
            int a,b,c;
            int result = sscanf(line, "f %d %d %d", &a, &b, &c);
            if (result == 3)
            {
                pointsMemory[pointsCount] = pointsMemory[a-1];
                pointsCount++;
                pointsMemory[pointsCount] = pointsMemory[b-1];
                pointsCount++;
                pointsMemory[pointsCount] = pointsMemory[c-1];
                pointsCount++;
            }
        }

        if (line[0] =='v'){
            struct Point a;
            int result = sscanf(line, "v %f %f %f", &a.x, &a.y, &a.z);
            a.x = a.x;
            a.y = a.y;
            a.z = a.z;
            if (result == 3)
            {
                pointsMemory[pointsCount] = a;
                pointsCount++;
            }
        }
    }
    fclose(file);
}

int readOnlyVertices(const char* filename){
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Failed to open file: %s\n", filename);
        return;
    }
    char line[256];
    int vertices = verticesInObj(filename);
    pointsMemory = malloc(sizeof(struct Point) * (vertices));
    pointsCount = 0;
    while (fgets(line, sizeof(line), file))
    {
        if( (line[0]=='#') || (line[0]=='f')){
            continue;
        }

        if (line[0] =='v'){
            struct Point a;
            int result = sscanf(line, "v %f %f %f", &a.x, &a.y, &a.z);
            a.x = a.x;
            a.y = a.y;
            a.z = a.z;
            if (result == 3)
            {
                pointsMemory[pointsCount] = a;
                pointsCount++;
            }
        }
    }
    fclose(file);
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

float getXMin(struct Point* points, int N){
    float min = points[0].x;
    for (int i=0; i<N; i++)
    {
        struct Point point = points[i];
        float x = point.x;
        if (x < min)
            min = x;
    }
    return min;
}

float getXMax(struct Point* points, int N){
    float max = points[0].x;
    for (int i=0; i<N; i++)
    {
        struct Point point = points[i];
        float x = point.x;
        if (x > max)
            max = x;
    }
    return max;
}

float getYMin(struct Point* points, int N){
    float min = points[0].y;
    for (int i=0; i<N; i++)
    {
        struct Point point = points[i];
        float y = point.y;
        if (y < min)
            min = y;
    }
    return min;
}

float getYMax(struct Point* points, int N){
    float max = points[0].y;
    for (int i=0; i<N; i++)
    {
        struct Point point = points[i];
        float y = point.y;
        if (y > max)
            max = y;
    }
    return max;
}

float getZMin(struct Point* points, int N){
    float min = points[0].z;
    for (int i=0; i<N; i++)
    {
        struct Point point = points[i];
        float z = point.z;
        if (z < min)
            min = z;
    }
    return min;
}

float getZMax(struct Point* points, int N){
    float max = points[0].z;
    for (int i=0; i<N; i++)
    {
        struct Point point = points[i];
        float z = point.z;
        if (z > max)
            max = z;
    }
    return max;
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
        filenameOk = filename;
        return displayObj;
    }

    if (strstr(filename, "geographos.tab") != NULL)
    {
        return displayGeo;
    }

    if (strstr(filename, "eros") != NULL)
    {
        return displayEros;
    }

    printf("File format not supported: %s\n", filename);
    return NULL;
}

void endTri()
{
    if (trianglesMemory != NULL)
        free(trianglesMemory);
}