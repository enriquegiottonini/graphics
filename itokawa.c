/*
    Titulo: Proyecto Final Graficacion
    Descripcion: Proyecciones de objetos 3D a 2D, obtenidos de archivos
    Autores: Nan y Jorge
    Fecha: 5/18/2016
*/

// -lGLEW
#include <GL/glew.h>
// -lglut
#include <GL/glut.h>
#include <stdio.h>
//-lm
#include <math.h>
// -lgl

//Estructura triangulo para manejar los puntos de los archivos
struct Trianglew
{
	float p1[4], p2[4], p3[4];
};
struct Point{
	float x,y,z;
};
//Prototipos de funciones
void mat_mul(float[16], float[], int, float[]);
void mat_id(float[16]);
void mat_trans(float[16], float, float, float);
void mat_rotx(float[16], float);
void mat_rotx2(float[16], float, float);
void mat_roty(float[16], float);
void mat_roty2(float[16], float, float);
void mat_rotz(float[16], float);
void mat_rotz2(float[16], float, float);
void mat_scale(float[16], float, float, float);
void mat_orto(float[16], int plano);
void mat_iso(float mat[16], float a, float b, float c);
void proyectar(float[16], struct Trianglew, int);
void calc_mat(float [16], int);
void isometrica();
void abrir_archivo_y_procesa_puntos(struct Trianglew[]);
void abrir_archivo_y_procesa_caras(struct Trianglew[]);
void setup();
void display();
void draw_pixel(float*, int, int, float, float, float);
//Numero de triangulos en el archivo itokawa
int N=10000;
struct Trianglew triangles[49152];
struct Point points[7381];
//Main
int main(int argc, char *argv[])
{
	abrir_archivo_y_procesa_puntos(triangles);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(600,600);
	glutCreateWindow("Itokawa");

	setup();
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}
//Funcion de dibujado de OpenGL
void display(){
    //Limpiamos el display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Declaracion de la matriz compuesta
	float m[16];
	//Empezamos la interfaz de openGL para dibujar lineas
	glBegin(GL_LINES);
	//Definimos color de linea blanco
	glColor3f(1.0f, 1.0f, 1.0f);
	//para 3 planos
    //Plano de proyeccion (0 = xy, 1 = xz, 2 = yz)
	for(int j=0; j<3; ++j){
		//Calculamos la matriz compuesta
		calc_mat(m, j);
		//Proyectamos y dibujamos todos los puntos
		for(int i=0; i<N; ++i){
			proyectar(m, triangles[i],j);
		}
	}
	isometrica();
	//Cerramos la interfaz de openGL
	glEnd();
	//Swapeamos los buffers ((8
	glutSwapBuffers();
}
//Multiplicacion de matrices
//m2 no es de longitud fija ya que a esta funcion se le puede enviar un vector tambien
void mat_mul(float m1[16], float m2[], int n, float mat[]){
	float row[n];
    for (int j=0; j<4; ++j){
        for (int i=0; i<n; ++i) {
            row[i] = 0;
            for (int l=0; l<4; ++l)
                row[i] += m1[j*4+l] * m2[l*n+i];
        }
        for (int i=0; i<n; ++i)
            mat[j*n+i] = row[i];
    }

}
//Matriz identidad
void mat_id(float mat[16]){

              mat[1]  = mat[2] = mat[3]  =
    mat[4]  =           mat[6] = mat[7]  =
    mat[8]  = mat[9]  =          mat[11] =
    mat[12] = mat[13] = mat[14]          = 0;

    mat[0]  = mat[5]  = mat[10]= mat[15] = 1;

}
//Matriz de translacion
void mat_trans(float mat[16], float dx, float dy, float dz){
    float t[16] = {
        1, 0, 0, dx,
        0, 1, 0, dy,
        0, 0, 1, dz,
        0, 0, 0, 1 };

    mat_mul(mat, t, 4, mat);

}
//Matriz de rotacion sobre el eje x
void mat_rotx(float mat[16], float theta){
    mat_rotx2(mat, sin(theta), cos(theta));
}
//Matriz de rotacion sobre el eje x
void mat_rotx2(float mat[16], float sintheta, float costheta){
    float rx[16] = {
        1,        0,         0, 0,
        0, costheta, -sintheta, 0,
        0, sintheta,  costheta, 0,
        0,        0,         0, 1 };
    mat_mul(mat, rx, 4, mat);
}
//Matriz de rotacion sobre el eje y
void mat_roty(float mat[16], float theta){
    mat_roty2(mat, sin(theta), cos(theta));
}
//Matriz de rotacion sobre el eje y
void mat_roty2(float mat[16], float sintheta, float costheta){
    float ry[16] = {
       costheta, 0, sintheta, 0,
              0, 1,        0, 0,
      -sintheta, 0, costheta, 0,
              0, 0,        0, 1 };
    mat_mul(mat, ry, 4, mat);
}
//Matriz de rotacion sobre el eje z
void mat_rotz(float mat[16], float theta){
    mat_rotz2(mat, sin(theta), cos(theta));
}
//Matriz de rotacion sobre el eje z
void mat_rotz2(float mat[16], float sintheta, float costheta){
    float rz[16] = {
      costheta, -sintheta, 0, 0,
      sintheta,  costheta, 0, 0,
             0,         0, 1, 0,
             0,         0, 0, 1 };
    mat_mul(mat, rz, 4, mat);

}
//Matriz de escalamiento
void mat_scale(float mat[16], float sx, float sy, float sz){
    float s[16] = {
        sx,  0,  0, 0,
         0, sy,  0, 0,
         0,  0, sz, 0,
         0,  0,  0, 1 };
    mat_mul(mat, s, 4, mat);

}

void mat_orto(float mat[16], int plano){
	//0: x->x, y->y
	//1: x->x, z->y
	//2: y->x, z->y
	float t[16] = {
		(plano==2?0:1), (plano==2?1:0),            0, 0,
		           0, (plano==0?1:0), (plano==0?0:1), 0,
		           0,            0,            0, 0,
		           0,            0,            0, 1
	};
	mat_mul(mat, t, 4, mat);
}

void mat_iso(float mat[16], float a, float b, float c){
	float i[16] = {
		c/sqrt(a*a+c*c), 0, -a/sqrt(a*a+c*c), 0,
		-a*b/sqrt((a*a+c*c)*(a*a+b*b+c*c)), sqrt((a*a+c*c)/(a*a+b*b+c*c)), -c*b/sqrt((a*a+c*c)*(a*a+b*b+c*c)), 0,
		0,0,0,0,
		0,0,0,1
	};
	mat_mul(mat, i, 4, mat);
}

//Funcion que calcula viewport, lo cual nos permite deducir la razon de escalamiento entre la pantalla y el objeto
void calc_viewport(float *xmin, float *ymin, float *xmax, float *ymax,int plano){
    //Definimos indices para determinar en que plano buscaremos los puntos
    int j,k;
    switch(plano){
        case 0:
            j=0,k=1;
            break;
        case 1:
            j=0,k=2;
            break;
        case 2:
            j = 1,k=2;
            break;
        default:
            printf("No deberia de estar pasando esto");
    }
    //Buscamos el minimo y maximo de los puntos en ambos ejes.
	for (int i = 0; i < N; ++i)
	{
		if(triangles[i].p1[j] < *xmin) *xmin = triangles[i].p1[j];
		if(triangles[i].p1[k] < *ymin) *ymin = triangles[i].p1[k];
		if(triangles[i].p1[j] > *xmax) *xmax = triangles[i].p1[j];
		if(triangles[i].p1[k] > *ymax) *ymax = triangles[i].p1[k];

		if(triangles[i].p2[j] < *xmin) *xmin = triangles[i].p2[j];
		if(triangles[i].p2[k] < *ymin) *ymin = triangles[i].p2[k];
		if(triangles[i].p2[j] > *xmax) *xmax = triangles[i].p2[j];
		if(triangles[i].p2[k] > *ymax) *ymax = triangles[i].p2[k];

		if(triangles[i].p3[j] < *xmin) *xmin = triangles[i].p3[j];
		if(triangles[i].p3[k] < *ymin) *ymin = triangles[i].p3[k];
		if(triangles[i].p3[j] > *xmax) *xmax = triangles[i].p3[j];
		if(triangles[i].p3[k] > *ymax) *ymax = triangles[i].p3[k];
	}
}
//Funcion que calcula la matriz compuesta que usaremos para proyectar en pantalla.
void calc_mat(float m[16],int plano){
    //definimos la matriz M como la identidad
	mat_id(m);
    //Definimos las variables maximas y minimas que cubre nuestro objeto.
	float amin, bmin, amax, bmax;
	//Calculamos sus valores
	calc_viewport(&amin, &bmin, &amax, &bmax,plano);
	//Calculamos el ancho y alto del objeto
	float w = (amax-amin);
	float h = (bmax-bmin);
	//Calculamos el factor de escalamiento para la transformacion windows a viewport
    //width = height = 1
	float sa = 1/w, sb = 1/h;
	//Se comparan las escalas en 'a' y 'b' y se elije la menor, para que el objeto conserve su escala original
	if(sa < sb) sb = sa;
	else sa = sb;
    //Calculamos las distancias necesarias para que el objeto quede centrado
    float x = 1-plano%2;
    float y = plano/2;
	float da = (1-(w*sa))/2-x, db = -((h*sb)-1)/2-y;
	//Calculamos la matriz de transformacion compuesta
	mat_trans(m, da, db,0);
	mat_scale(m, sa, sb, 1);
	mat_trans(m, -amin, -bmin, 0);
	mat_orto(m, plano);
	//DEBUG: imprimimos los puntos
	for(int i=0; i<4; ++i)
		printf("[%f,%f,%f,%f]\n", m[4*i], m[4*i+1], m[4*i+2], m[4*i+3]);

}
//Funcion que proyecta y dibuja los triangulos en el plano deseado
void proyectar(float m[16], struct Trianglew t, int plano){
    ///TODO:Proyeccion axonometrica
    //arreglos auxiliares para dibujar puntos
	float r1[4], r2[4], r3[4];
    //multiplicamos los puntos de el triangulo por la matriz compuesta
	mat_mul(m, t.p1, 1, r1);
	mat_mul(m, t.p2, 1, r2);
	mat_mul(m, t.p3, 1, r3);

	//Dibujamos los puntos, usando la variable plano para decidir sobre cual plano proyectar


	glVertex3f(r1[0], r1[1], 0);
    glVertex3f(r2[0], r2[1], 0);
    glVertex3f(r2[0], r2[1], 0);
    glVertex3f(r3[0], r3[1], 0);
    glVertex3f(r3[0], r3[1], 0);
	glVertex3f(r1[0], r1[1], 0);
}

void isometrica(){
	float m[16] = {};
	mat_id(m);
	mat_iso(m, 1,1,1);
	float puntos[N*12];
	float xmin, ymin, xmax, ymax;
	for (int i = 0; i < N; ++i){
		mat_mul(m, triangles[i].p1, 1, puntos+i*12);
		xmin = puntos[i*12] < xmin ? puntos[i*12] : xmin;
		xmax = puntos[i*12] > xmax ? puntos[i*12] : xmax;
		ymin = puntos[i*12+1] < ymin ? puntos[i*12+1] : ymin;
		ymax = puntos[i*12+1] > ymax ? puntos[i*12+1] : ymax;
		mat_mul(m, triangles[i].p2, 1, puntos+i*12+4);
		xmin = puntos[i*4] < xmin ? puntos[i*4] : xmin;
		xmax = puntos[i*12+4] > xmax ? puntos[i*12+4] : xmax;
		ymin = puntos[i*12+4+1] < ymin ? puntos[i*12+4+1] : ymin;
		ymax = puntos[i*12+4+1] > ymax ? puntos[i*12+4+1] : ymax;
		mat_mul(m, triangles[i].p3, 1, puntos+i*12+8);
		xmin = puntos[i*12+8] < xmin ? puntos[i*12+8] : xmin;
		xmax = puntos[i*12+8] > xmax ? puntos[i*12+8] : xmax;
		ymin = puntos[i*12+8+1] < ymin ? puntos[i*12+8+1] : ymin;
		ymax = puntos[i*12+8+1] > ymax ? puntos[i*12+8+1] : ymax;
	}
	mat_id(m);
	float w = (xmax-xmin);
	float h = (ymax-ymin);
	//Calculamos el factor de escalamiento para la transformacion windows a viewport
    //width = height = 1
	float sa = 1/w, sb = 1/h;
	//Se comparan las escalas en 'a' y 'b' y se elije la menor, para que el objeto conserve su escala original
	if(sa < sb) sb = sa;
	else sa = sb;
    //Calculamos las distancias necesarias para que el objeto quede centrado
	float da = (1-(w*sa))/2, db = -((h*sb)-1)/2-1;
	//Calculamos la matriz de transformacion compuesta
	mat_trans(m, da, db,0);
	mat_scale(m, sa, sb, 1);
	mat_trans(m, -xmin, -ymin, 0);

	for(int i=0; i<4; ++i)
		printf("[%f,%f,%f,%f]\n", m[4*i], m[4*i+1], m[4*i+2], m[4*i+3]);


	float r1[4], r2[4], r3[4];
	for(int i=0; i<N; ++i){
		mat_mul(m, puntos+i*12, 1, r1);
		mat_mul(m, puntos+i*12+4, 1, r2);
		mat_mul(m, puntos+i*12+8, 1, r3);

		//Dibujamos los puntos, usando la variable plano para decidir sobre cual plano proyectar


		glVertex3f(r1[0], r1[1], 0);
	    glVertex3f(r2[0], r2[1], 0);
	    glVertex3f(r2[0], r2[1], 0);
	    glVertex3f(r3[0], r3[1], 0);
	    glVertex3f(r3[0], r3[1], 0);
		glVertex3f(r1[0], r1[1], 0);
	}

}

//Preparar la interfaz de openGL
void setup(){
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

//Funcion para procesar los datos almacenados en los archivos
void abrir_archivo_y_procesa_puntos(struct Trianglew triangles[]){
	//Lee archivos que utilizan un sistema de tres puntos
    //Declaramos las variables necesarias para procesar archivos
	FILE *file;
	float buff;
	//Abrimos el archivo con los puntos
	file = fopen("OBJETOS-3D/itokawa_f0049152.tri", "r");
	int i=0;
	//Leemos todo el archivo
	while(fscanf(file, "%f", &buff) != EOF){
        //Obtenemos las tres coordenadas del primer punto del triangulo
		triangles[i].p1[0] = buff;
		fscanf(file, "%f", &buff);
		triangles[i].p1[1] = buff;
		fscanf(file, "%f", &buff);
		triangles[i].p1[2] = buff;
		triangles[i].p1[3] = 1;
		fscanf(file, "%f", &buff);
        //Hacemos lo mismo para el segundo punto
		triangles[i].p2[0] = buff;
		fscanf(file, "%f", &buff);
		triangles[i].p2[1] = buff;
		fscanf(file, "%f", &buff);
		triangles[i].p2[2] = buff;
		triangles[i].p2[3] = 1;
		fscanf(file, "%f", &buff);
		//Y lo mismo para el tercer punto
		triangles[i].p3[0] = buff;
		fscanf(file, "%f", &buff);
		triangles[i].p3[1] = buff;
		fscanf(file, "%f", &buff);
		triangles[i].p3[2] = buff;
		triangles[i].p3[3] = 1;
		fscanf(file, "%f", &buff);
		++i;
	}
	//Cerramos el archivo
	fclose(file);
}
void abrir_archivo_y_procesa_caras(struct Trianglew triangles[]){
	//Lee archivos que utilizan un sistema de caras y vertices

    //Declaramos las variables necesarias para procesar archivos
	FILE *file;
	float floatBuffer;
	char charBuffer;
	int intBuffer;
	struct Point puntos[20002];
	//Abrimos el archivo con los puntos
	file = fopen("OBJETOS-3D/QueSoy1.obj", "r");
	int i=0;
	//Leemos todo el archivo
	while(fscanf(file, "%c", &charBuffer) != 'v' && i<10000){
        //Obtenemos las tres coordenadas del vertice

		fscanf(file, "%f", &floatBuffer);
		puntos[i].x = floatBuffer;
		fscanf(file, "%f", &floatBuffer);
		puntos[i].y = floatBuffer;
		fscanf(file, "%f", &floatBuffer);
		puntos[i].z = floatBuffer;
		printf("[%f,%f,%f], %f\n", puntos[i].x, puntos[i].y, puntos[i].z, i);
		++i;
	}
	i = 0;
	//Asociamos los vertices para formar un triangulo
	do{
		//fscanf(file,"%d",&intBuffer);
		triangles[i].p1[0] = puntos[i].x;
		triangles[i].p1[1] = puntos[i].y;
		triangles[i].p1[2] = puntos[i].z;
		triangles[i].p1[3] = 1;

		//fscanf(file,"%d",&intBuffer);
		triangles[i].p2[0] = puntos[i].x;
		triangles[i].p2[1] = puntos[i].y;
		triangles[i].p2[2] = puntos[i].z;
		triangles[i].p2[3] = 1;

		//fscanf(file,"%d",&intBuffer);
		triangles[i].p3[0] = puntos[i].x;
		triangles[i].p3[1] = puntos[i].y;
		triangles[i].p3[2] = puntos[i].z;
		triangles[i].p3[3] = 1;
		++i;
	}while(fscanf(file,"%c",&charBuffer) != EOF && i<10000);
	//Cerramos el archivo
	fclose(file);
}
///TODO:Realmente no hace nada en este momento
/* 
void abrir_archivo_y_procesa_esfericos(struct Trianglew triangles[]){
	//Lee archivos que utilizan sistema esferico de coordenadas
    //Declaramos las variables necesarias para procesar archivos
	FILE *file;
	float buff;
	//Abrimos el archivo con los puntos
	file = fopen("OBJETOS-3D/253mathilde.tab", "r");
	int i=0;
	float latitude,longitude,radius;

	
	//Leemos todo el archivo
	while(fscanf(file, "%f", &buff) != EOF){
        //Obtenemos las tres coordenadas del primer punto del triangulo
		latitude = buff;
		fscanf(file,"%f",&buff);
		longitude = buff;
		fscanf(file,"%f",&buff);
		radius = buff;
		++i;
		points[i].x = radius*sintheta(latitude)*costheta(longitude);
		points[i].y = radius*sintheta(latitude)*sintheta(longitude);
		points[i].z = rcos(latitude);


	}
	//Cerramos el archivo
	fclose(file);
}
*/