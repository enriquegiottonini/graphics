#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <GL/glut.h>

// General three-dimensional transformation pipeline, from modeling coordinates (MC) to world coordinates (WC) to
// viewing coordinates (VC) to projection coordinates (PC) to normalized coordinates (NC) and, ultimately, to device
// coordinates (DC)

void init (void);
void displayFcn (void);
void reshapeFcn (GLint newWidth, GLint newHeight);


#endif