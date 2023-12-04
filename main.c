// To run: make run
// To test: make test
#include "utils.h"

int main (int argc, char** argv)
{
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Proyecto Final");
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    const char* itokawa = "OBJETOS-3D/itokawa_f0049152.tri";
    const char* cactus = "OBJETOS-3D/QueSoy2.obj";

    const char* filename = cactus;
    void (*displayFunc)(void) = getDisplayFunc(filename);
    
    glutDisplayFunc(displayFunc);
	glutMainLoop();

    endTri();

    return 0;
}