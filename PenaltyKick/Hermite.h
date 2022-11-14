#pragma once
#include "Lib.h"

typedef struct Figura {
	GLuint VAO;
	GLuint VBO_G;
	GLuint VBO_C;
	GLuint EBO_indici;
	int nTriangles;
	// Vertici
	vector<vec3> vertici;
	vector<vec3> CP;
	vector<vec4> colors;
	vector<vec4> colCP;
	vector<int> indici;
	// Numero vertici
	int nv;
	//Matrice di Modellazione: Traslazione*Rotazione*Scala
	mat4 Model;
	vec4 corner_b_obj;
	vec4 corner_t_obj;
	vec4 corner_b;
	vec4 corner_t;
	bool active;
	int sceltaFS;
} Figura;

class Hermite{
	public:
		void costruisci_curva(float x, float y, Figura* fig);
		void costruisci_formaHermite(vec4 color_top, vec4 color_bot, Figura* curva);
	private:
		void InterpolazioneHermite(float* t, Figura* Fig, vec4 color_top, vec4 color_bot);
		float DY(int i, float* t);
		float DX(int i, float* t);
		float dy(int i, float* t, float Tens, float Bias, float Cont, Figura* Fig);
		float dx(int i, float* t, float Tens, float Bias, float Cont, Figura* Fig);
};