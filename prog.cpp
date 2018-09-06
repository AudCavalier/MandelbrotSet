#include <iostream>
#include <stdio.h>
#include <complex>
#include <omp.h>
#include <sys/time.h>
#include "pix.h"

#define width 1024
#define height 900

pix pixMatrix[height][width];

GLuint tex;

struct timeval t0, t1;

void TrazaTiempo(char*, struct timeval*, struct timeval*);
void drawFunc(void);
void mandelbrot(void);

int main(int argc, char **argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutCreateWindow("Mandelbrot en esteroides");
	
	glutDisplayFunc(drawFunc);

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glClearColor(0, 0, 0, 0);
	glLoadIdentity();
	glColor3f(1, 1, 1);
	
	glutMainLoop();
	return 0;
}

//Esta función se usa para dibujar la gráfica con OpenGL
void drawFunc(){
	//Llamamos la función de Mandelbrot: por alguna razón OpenGL la llama 2 veces, esto lo googlee pero no encontré alguna explicación en concreto
	mandelbrot();
	//Cosas de texturas... básicamente la matriz se convierte en una textura
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixMatrix);
	glClear(GL_COLOR_BUFFER_BIT);	
	glBegin(GL_QUADS);
	glTexCoord2f(1, 0);
	glVertex2f(1, -1);
	glTexCoord2f(1, 1);
	glVertex2f(1, 1);
	glTexCoord2f(0 ,1);
	glVertex2f(-1, 1);
	glTexCoord2f(0, 0);
	glVertex2f(-1, -1);
	glEnd();
	
	glFlush();
	glutSwapBuffers();
}

void mandelbrot(){
/*DEFINICIÓN: M = { c | c <- lim (n->inf) Zn != inf}
 Z0 = c; 
 Zn+1 = Zn²+C

 Básicamente por definición si el valor absoluto de Z (es decir, su distancia desde 0+0i) se vuelve mayor a 2, nunca regresará a un lugar más cercano a 2, pero rápidamente 
 escapará a infinito. (según el VLOG de donde vi esto, esto se puede demostrar facilmente, tarea para Reynoso....)
*/	
	struct timeval t0, t1;
	double MinRe = -2.0; //EN PRINCIPIO SI FUNCIONA CON ESTOS NUMEROS DEBERÍA FUNCIONAR CON CUALQUIERA
	double MaxRe = 1.0;
	double MinIm = -1.2;
	double MaxIm = 1.2;
	int itermax=500;
	bool in;
	
	/*
	SI EL PIXEL EN LA ESQUINA SUPERIOR IZQUIERDA TIENE COORDENADAS (0,0) Y EL PIXEL EN LA ESQUINA INFERIOR DERECHA TIENE COORDENADAS (ImWidth-1, ImHeight-1), 
	para obtener el número complejo equivalente para una coordenada (X, Y) de los conjuhntos de valores anteriores se usan estas formulas:
	c_re = MinRe + x*(MaxRe-MinRe)/(Width);
	c_im = MaxIm - y*(MaxIm-MinIm)/(Height);
	*/
	gettimeofday(&t0, 0);

	//EL FOR PRINCIPAL, Y EL MÁS IMPORTANTE Y EL QUE ME SALVA
	#pragma omp parallel for schedule(dynamic)
	for(int y=0; y<900; y++){
		//EN PRINCIPIO, iteramos sobre todos los pixeles de la imagen para calcular a que numero complejo corresponden y si pertenece al conjunto
		for(int x=0; x<1024; x++){
			in=true;
			//REVISANDO SI EL NÚMERO PERTENECE AL CONJUNTO
			std::complex<double> c(MinRe+(x*(MaxRe-MinRe)/width), MaxIm+(y*(MinIm-MaxIm)/height));
			std::complex<double> Z(0.0, 0.0);
			for(int n=0; n<itermax; n++){
				if(abs(Z)>2){
					in=false;
					break;
				}
				Z=(Z*Z)+c;
			}
			if(in){
				pixMatrix[y][x].r=0;
				pixMatrix[y][x].g=0;
				pixMatrix[y][x].b=0;
			}else{
				pixMatrix[y][x].r=127;
				pixMatrix[y][x].g=153;
				pixMatrix[y][x].b=191;
			}
		}
	}
	gettimeofday(&t1, 0);
	printf("BENCHMARK: \n");
	TrazaTiempo("Tiempo de ejecución: ", &t0, &t1);
}

void TrazaTiempo(char * pTexto, struct timeval *pt0, struct timeval *pt1){
	double tej;
	int nthr=0;
	#ifdef _OPENMP
		#pragma omp parallel
		{
			nthr = omp_get_num_threads();
		}
	#endif
	tej = (pt1->tv_sec - pt0->tv_sec) + (pt1->tv_usec - pt0->tv_usec) / 1e6;
	printf("%s = %10.3f ms (%d threads)\n",pTexto, tej*1000, nthr);
}

//COMPILACIÓN:  g++ prog.cpp -fopenmp -o prog -lglut -lGL
