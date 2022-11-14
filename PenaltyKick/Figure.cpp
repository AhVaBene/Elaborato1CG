#include "Hermite.h"

#define  PI   3.14159265358979323846

void crea_VAO_CP(Figura* fig)
{
	glGenVertexArrays(1, &fig->VAO);
	glBindVertexArray(fig->VAO);
	//Genero , rendo attivo, riempio il VBO della geometria dei vertici di COntrollo
	glGenBuffers(1, &fig->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, fig->CP.size() * sizeof(vec3), fig->CP.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	//Genero , rendo attivo, riempio il VBO dei colori nei vertici di controllo
	glGenBuffers(1, &fig->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, fig->colCP.size() * sizeof(vec4), fig->colCP.data(), GL_DYNAMIC_DRAW);
	//Adesso carico il VBO dei colori nel layer 2
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
}

void crea_VAO_Vector(Figura* fig)
{
	glGenVertexArrays(1, &fig->VAO);
	glBindVertexArray(fig->VAO);
	//Genero , rendo attivo, riempio il VBO della geometria dei vertici
	glGenBuffers(1, &fig->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, fig->vertici.size() * sizeof(vec3), fig->vertici.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	//Genero , rendo attivo, riempio il VBO dei colori
	glGenBuffers(1, &fig->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, fig->colors.size() * sizeof(vec4), fig->colors.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO dei colori nel layer 2
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
}

void costruisci_cerchio(vec4 color_top, vec4 color_bot, Figura* cerchio)
{
	int i;
	float stepA = (2 * PI) / cerchio->nTriangles;
	float t;

	cerchio->vertici.push_back(vec3(0.0, 0.0, 0.0));
	cerchio->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));

	for (i = 0; i <= cerchio->nTriangles; i++)
	{
		t = (float)i * stepA;

		cerchio->vertici.push_back(vec3(cos(t), sin(t), 0.0));
		//Colore 
		if (i % 3 == 0) {
			cerchio->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
		}
		else {
			cerchio->colors.push_back(vec4(color_bot.r, color_bot.g, color_bot.b, color_bot.a));
		}

	}
	cerchio->nv = cerchio->vertici.size();
	//Costruisco xmin,ymin, xmax,ymax peer identificare il Bounding Box della palla
	float xmax = 0.0;
	float xmin = 0.0;
	float ymax = 0.0;
	float ymin = 0.0;
	for (i = 1; i < cerchio->nv; i++) {
		if (cerchio->vertici[i].x < xmin)
			xmin = cerchio->vertici[i].x;
	}
	for (i = 1; i < cerchio->nv; i++) {
		if (cerchio->vertici[i].x > xmax)
			xmax = cerchio->vertici[i].x;
	}
	for (i = 1; i < cerchio->nv; i++) {
		if (cerchio->vertici[i].y <= ymin)
			ymin = cerchio->vertici[i].y;
	}
	for (i = 1; i < cerchio->nv; i++) {
		if (cerchio->vertici[i].y > ymax)
			ymax = cerchio->vertici[i].y;
	}
	//Aggiorno i valori del corner pi� in basso a sinistra (corner_b) e del corner pi� in alto a destra (conrner_t)
	cerchio->corner_b_obj = vec4(xmin, ymin, 0.0, 1.0);
	cerchio->corner_t_obj = vec4(xmax, ymax, 0.0, 1.0);

	cerchio->vertici.push_back(vec3(xmin, ymin, 0.0));
	cerchio->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	cerchio->vertici.push_back(vec3(xmax, ymin, 0.0));
	cerchio->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	cerchio->vertici.push_back(vec3(xmax, ymax, 0.0));
	cerchio->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	cerchio->vertici.push_back(vec3(xmin, ymin, 0.0));
	cerchio->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	cerchio->vertici.push_back(vec3(xmin, ymax, 0.0));
	cerchio->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	cerchio->vertici.push_back(vec3(xmax, ymax, 0.0));
	cerchio->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));

	//Aggiorno il numero dei vertici della figura
	cerchio->nv = cerchio->vertici.size();
	cerchio->sceltaFS = 0;
}



void costruisci_prato(vec4 color_top, vec4 color_bot, Figura* prato , int width, int height)
{
	prato->vertici.push_back(vec3(0.0, 0.0, 0.0));
	prato->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	prato->vertici.push_back(vec3(0.0, 1.0, 0.0));
	prato->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	prato->vertici.push_back(vec3(1.0, 0.0, 0.0));
	prato->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	prato->vertici.push_back(vec3(1.0, 1.0, 0.0));
	prato->colors.push_back(vec4(color_bot.r, color_bot.g, color_bot.b, color_bot.a));

	prato->Model = mat4(1.0);
	prato->Model = scale(prato->Model, vec3(float(width), float(height) / 3.0, 1.0));
	prato->nv = prato->vertici.size();
	prato->sceltaFS = 0;
}

void costruisci_cielo(vec4 color_top, vec4 color_bot, Figura* cielo, int width, int height)
{
	cielo->vertici.push_back(vec3(0.0, 0.0, 0.0));
	cielo->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	cielo->vertici.push_back(vec3(0.0, 1.0, 0.0));
	cielo->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	cielo->vertici.push_back(vec3(1.0, 0.0, 0.0));
	cielo->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	cielo->vertici.push_back(vec3(1.0, 1.0, 0.0));
	cielo->colors.push_back(vec4(color_bot.r, color_bot.g, color_bot.b, color_bot.a));

	cielo->Model = mat4(1.0);
	cielo->Model = translate(cielo->Model, vec3(0.0, float(height) / 3, 0.0));
	cielo->Model = scale(cielo->Model, vec3(float(width), 2.0 * float(height) / 3.0, 1.0));
	cielo->nv = cielo->vertici.size();
	cielo->sceltaFS = 1;
}

void costruisci_porta(vec4 color_top, vec4 color_bot, Figura* porta, int width, int height) {
	//palo sinistro
	porta->vertici.push_back(vec3(-1.0, -1.0, 0.0));
	porta->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	porta->vertici.push_back(vec3(-0.94, -1.0, 0.0));
	porta->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	porta->vertici.push_back(vec3(-1.0, 0.5, 0.0));
	porta->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	porta->vertici.push_back(vec3(-0.94, 0.5, 0.0));
	porta->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	//palo destro
	porta->vertici.push_back(vec3(1.0, -1.0, 0.0));
	porta->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	porta->vertici.push_back(vec3(0.94, -1.0, 0.0));
	porta->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	porta->vertici.push_back(vec3(1.0, 0.5, 0.0));
	porta->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	porta->vertici.push_back(vec3(0.94, 0.5, 0.0));
	porta->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	//traversa
	porta->vertici.push_back(vec3(0.94, 0.5, 0.0));
	porta->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	porta->vertici.push_back(vec3(0.94, 0.40, 0.0));
	porta->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	porta->vertici.push_back(vec3(-0.94, 0.5, 0.0));
	porta->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	porta->vertici.push_back(vec3(-0.94, 0.40, 0.0));
	porta->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	//rete(verticale)
	for (float i = -0.90; i <= 0.90; i += 0.20) {
		porta->vertici.push_back(vec3(i, 0.4, 0.0));
		porta->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
		porta->vertici.push_back(vec3(i, -1.0, 0.0));
		porta->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	}
	//rete(orizzontale)
	for (float i = -0.90; i <= 0.5; i += 0.20) {
		porta->vertici.push_back(vec3(-0.94, i, 0.0));
		porta->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
		porta->vertici.push_back(vec3(0.94, i, 0.0));
		porta->colors.push_back(vec4(color_top.r, color_top.g, color_top.b, color_top.a));
	}
	//metto i vertici del "corner"(angolo basso a destra e altezza traversa)
	porta->corner_b_obj = vec4(0.94, -1.0, 0.0, 1.0);
	porta->corner_t_obj = vec4(1.0, 0.40, 0.0, 1.0);
	porta->Model = mat4(1.0);
	porta->Model = translate(porta->Model, vec3((float)width / 2.0, (float)height / 1.75, 0.0));
	porta->Model = scale(porta->Model, vec3((float)width / 3.0, (float)height / 4.0, 1.0));
	porta->corner_b = porta->corner_b_obj;
	porta->corner_t = porta->corner_t_obj;
	porta->corner_b = porta->Model * porta->corner_b;
	porta->corner_t = porta->Model * porta->corner_t;
	porta->nv = porta->vertici.size();
	porta->sceltaFS = 0;
}

void costruisci_portiere(Figura* piede, Figura* gamba, Figura* corpo, Figura* mano, Figura* testa) {
	Hermite geom;
	vec4 col_top = vec4{ 102.0 / 255.0, 51.0 / 255.0, 0.0 / 255.0, 1.0000 };
	vec4 col_bottom = vec4{ 1.0, 0.0, 0.0, 1.0000 };
	geom.costruisci_curva(0.5, -0.5, piede);
	geom.costruisci_curva(0.5, -0.2, piede);
	geom.costruisci_curva(1.0, -0.2, piede);
	geom.costruisci_curva(1.0, -1.0, piede);
	geom.costruisci_curva(-0.4, -1.0, piede);
	geom.costruisci_curva(-0.4, -0.5, piede);
	geom.costruisci_curva(0.5, -0.5, piede);
	geom.costruisci_formaHermite(col_top, col_bottom, piede);
	piede->sceltaFS = 0;
	//Costruisco xmin,ymin, xmax,ymax peer identificare il Bounding Box del piede
	float xmax = 1.0;
	float xmin = -1.0;
	float ymax = 1.0;
	float ymin = -1.0;
	//Aggiorno i valori del corner pi� in basso a sinistra (corner_b) e del corner pi� in alto a destra (conrner_t)
	piede->corner_b_obj = vec4(xmin, ymin, 0.0, 1.0);
	piede->corner_t_obj = vec4(xmax, ymax, 0.0, 1.0);
	piede->vertici.push_back(vec3(xmin, ymin, 0.0));
	piede->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	piede->vertici.push_back(vec3(xmax, ymin, 0.0));
	piede->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	piede->vertici.push_back(vec3(xmax, ymax, 0.0));
	piede->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	piede->vertici.push_back(vec3(xmin, ymin, 0.0));
	piede->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	piede->vertici.push_back(vec3(xmin, ymax, 0.0));
	piede->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	piede->vertici.push_back(vec3(xmax, ymax, 0.0));
	piede->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));

	//Aggiorno il numero dei vertici della figura
	piede->nv = piede->vertici.size();

	col_top = vec4{ 55.0 / 255.0, 45.0 / 255.0, 45.0 / 255.0, 1.0 };
	col_bottom = vec4{ 25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0 };
	geom.costruisci_curva(-0.3, 0, gamba);
	geom.costruisci_curva(-0.3, -1, gamba);
	geom.costruisci_curva(-1, -1, gamba);
	geom.costruisci_curva(-0.7, 1, gamba);
	geom.costruisci_curva(0.7, 1, gamba);
	geom.costruisci_formaHermite(col_top, col_bottom, gamba);
	gamba->sceltaFS = 0;

	col_top = vec4{ 153.0 / 255.0, 0.0 / 255.0, 0.0 / 255.0, 1.0 };
	col_bottom = vec4{ 153.0 / 255.0, 0.0 / 255.0, 0.0 / 255.0, 1.0 };
	geom.costruisci_curva(-0.143, 0.286, corpo);
	geom.costruisci_curva(-0.600, -0.428, corpo);
	geom.costruisci_curva(-.85, -0.286, corpo);
	geom.costruisci_curva(-0.500, 0.500, corpo);
	geom.costruisci_curva(-0.143, 0.857, corpo);
	geom.costruisci_curva(0.429, 1.0, corpo);
	geom.costruisci_curva(.8, -1.0, corpo);
	geom.costruisci_curva(0.1, -1.0, corpo);
	geom.costruisci_formaHermite(col_top, col_bottom, corpo);
	corpo->sceltaFS = 0;

	col_top = vec4{ 153.0 / 255.0, 153.0 / 255.0, 0.0 / 255.0, 1.0 };
	col_bottom = vec4{ 255.0 / 255.0, 153.0 / 255.0, 100.0 / 255.0, 1.0 };
	mano->nTriangles = 30;
	costruisci_cerchio(col_top, col_bottom, mano);
	mano->sceltaFS = 0;

	col_top = vec4{ 255.0 / 255.0, 153.0 / 255.0, 153.0 / 255.0, 1.0 };
	col_bottom = vec4{ 255.0 / 255.0, 153.0 / 255.0, 153.0 / 255.0, 1.0 };
	testa->nTriangles = 30;
	costruisci_cerchio(col_top, col_bottom, testa);
}