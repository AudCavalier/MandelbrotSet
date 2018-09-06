//Practica para mi: definir archivos .h y agregarlos al .cpp
#ifndef PIX_H
#define PIX_H
#include <GL/glut.h>
#include <GL/freeglut.h>

//Como necesitaremos una matriz de pixeles, puesto que OpenGL no es "Seguro" con hilos, más vale hacer esta parte en paralelo y sólamente graficar la imagen
//La matriz de pixeles como el nombre lo indica, almacenará información de pixeles en cada posicion, cada pixel tiene información de color r, g, b (GLbyte)
typedef struct{
	GLbyte r;
	GLbyte g;
	GLbyte b;
}pix;

#endif