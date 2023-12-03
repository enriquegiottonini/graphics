#include <stdio.h>
#include <assert.h>
#include "utils.h"

void itReadsTriFile() {
    struct Triangle* triangles = readTrianglesFromFile("OBJETOS-3D/itokawa_f0049152.tri");
    assert(triangles != NULL);

    struct Triangle firstTriangle = triangles[0];
    assert(abs(firstTriangle.a.x - -0.151440) < 0.000001);
    assert(abs(firstTriangle.a.y - 0.081760) < 0.000001);
    assert(abs(firstTriangle.a.z - 0.074960) < 0.000001);

    struct Triangle lastTriangle = triangles[49151];
    // 0.132400 0.063050 -0.049180
    assert(abs(lastTriangle.c.x - 0.132400) < 0.000001);
    assert(abs(lastTriangle.c.y - 0.063050) < 0.000001);
    assert(abs(lastTriangle.c.z - -0.049180) < 0.000001);

    free(triangles);
}

int main() {
    itReadsTriFile();
    return 0;
}