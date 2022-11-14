#include <iostream>
#include <stdlib.h> 

#include "ShaderMaker.h"
#include "Lib.h"
#include "Hermite.h"
#include "GestioneTesto.h"
#include "Figure.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glew.h>
#include <GL/freeglut.h>

static unsigned int programId, programId_text;

mat4 Projection;
GLuint MatProj, MatModel, loctime, locres, lsceltafs, locgol;
unsigned int VAO_Text, VBO_Text;
int  width = 1200, height = 900, ngol = 0, potenza = 1;//1, 2, 3
float adder = 4, dx = 0, posx_palla = 0.0, posy_palla = 55.0, posx, posy, w_update, h_update, w_window, h_window;
bool collision = false;
char esitoTiro[100] = "";

Figura Piede, Gamba, Corpo, Mano, Testa, Retta, Palla, Prato, Porta, Cielo;
vector<Figura> Scena;

void updatePalla(int value) {
	posx_palla += posx / (60.0 - 10.0 * (float)potenza);
	posy_palla += posy / (60.0 - 10.0 * (float)potenza);
	if (posx_palla <= (float)width / 2.0 && posx_palla >= -(float)width / 2.0 && posy_palla <= posy) {
		Scena[4].active = true;
		glutTimerFunc(5, updatePalla, 0);
	}
	else {
		posy_palla = 55.0;
		posx_palla = 0;
		Scena[4].active = false;
		float estSx = -Scena[2].corner_b.x + ((float)width / 2.0);
		float estDx = Scena[2].corner_b.x - ((float)width / 2.0);
		float estUp = Scena[2].corner_t.y;
		float estDw = Scena[2].corner_b.y;
		if (posx < estDx && posx > estSx && posy < estUp && posy > estDw) { //se non c'è stata collisione col portiere e il tiro è dentro la porta, segno un gol
			if (!collision) {
				ngol++;
				strcat_s(esitoTiro, "GOL");
			}
		}
		else {
			strcat_s(esitoTiro, "FUORI");
		}
	}
	glutPostRedisplay();
}

void updatePortiere(int value) {
	if (dx == ((float)width / 4.0) || dx == -((float)width / 4.0) || rand() % 50 == 0) {
		adder *= -1.0;
	}
	dx += adder;
	glutTimerFunc(10, updatePortiere, 0);
	glutPostRedisplay();
}

bool checkCollision(Figura obj1, Figura obj2) {
	// guardo collisioni su asse x
	bool collisionX = obj1.corner_b.x <= obj2.corner_t.x &&
		obj1.corner_t.x >= obj2.corner_b.x;

	// guardo collisioni su asse y
	bool collisionY = obj1.corner_b.y <= obj2.corner_t.y &&
		obj1.corner_t.y >= obj2.corner_b.y;
	//Si ha collisione se c'� collisione sia nella direzione x che nella direzione y

	return collisionX && collisionY;
}

void INIT_VAO_Text(void)
{
	// configure VAO/VBO for texture quads
	// -----------------------------------
	glGenVertexArrays(1, &VAO_Text);
	glGenBuffers(1, &VBO_Text);
	glBindVertexArray(VAO_Text);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Text);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void INIT_VAO(void)
{
	//Costruisco il prato
	vec4 col_top = vec4{ 0.1333, 0.5451, 0.1333, 1.0000 };
	vec4 col_bottom = vec4{ 0.1333, 0.5451,0.1333, 1.0000 };
	costruisci_prato(col_top, col_bottom, &Prato, width, height);
	crea_VAO_Vector(&Prato);
	Scena.push_back(Prato);

	//Costruisco il cielo
	col_top = vec4{ 0.0, 0.0, 0.0, 1.0000 };
	col_bottom = vec4{ 0.0, 0.0,0.0, 1.0000 };
	costruisci_cielo(col_top, col_bottom, &Cielo, width, height);
	crea_VAO_Vector(&Cielo);
	Scena.push_back(Cielo);

	//Costruisco la porta
	col_top = { 214.0 / 255.0,215.0 / 255.0, 216.0 / 255.0, 1.0 };
	col_bottom = { 0.8, 1.0, 1.0, 1.0 };
	costruisci_porta(col_top, col_bottom, &Porta, width, height);
	crea_VAO_Vector(&Porta);
	Scena.push_back(Porta);

	//Costruisco il puntatore-retta
	Retta.CP.push_back(vec3((float)width / 2.0, 55.0, 0));
	Retta.colCP.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	Retta.CP.push_back(vec3(0.0, 0.0, 0));
	Retta.colCP.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	Retta.active = false;
	crea_VAO_CP(&Retta);
	Scena.push_back(Retta);

	//Costruisco il pallone
	col_top = { 255.0 / 255.0, 128.0 / 255.0, 0.0 / 255.0, 1.0 };
	col_bottom = { 102.0/255.0, 0.7, 0.7, 1.0 }; 
	Palla.nTriangles = 30;
	costruisci_cerchio(col_top, col_bottom, &Palla);
	crea_VAO_Vector(&Palla);
	Scena.push_back(Palla);

	//Costruisco il portiere
	costruisci_portiere(&Piede, &Gamba, &Corpo, &Mano, &Testa);
	crea_VAO_Vector(&Piede);
	Scena.push_back(Piede);
	Scena.push_back(Piede);
	crea_VAO_Vector(&Gamba);
	Scena.push_back(Gamba);
	Scena.push_back(Gamba);
	crea_VAO_Vector(&Corpo);
	Scena.push_back(Corpo);
	Scena.push_back(Corpo);
	crea_VAO_Vector(&Mano);
	Scena.push_back(Mano);
	Scena.push_back(Mano);
	crea_VAO_Vector(&Testa);
	Scena.push_back(Testa);

	MatProj = glGetUniformLocation(programId, "Projection");
	MatModel = glGetUniformLocation(programId, "Model");
	loctime = glGetUniformLocation(programId, "time");
	locres = glGetUniformLocation(programId, "resolution");
	lsceltafs = glGetUniformLocation(programId, "sceltaFS");
	locgol = glGetUniformLocation(programId, "gol");
	Projection = ortho(0.0f, float(width), 0.0f, float(height));
	glViewport(0, 0, width, height);
}

void INIT_SHADER(void)
{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader.glsl";
	char* fragmentShader = (char*)"fragmentShader.glsl";
	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);

	//Generazione del program shader per la gestione del testo
	vertexShader = (char*)"VertexShader_Text.glsl";
	fragmentShader = (char*)"FragmentShader_Text.glsl";

	programId_text = ShaderMaker::createProgram(vertexShader, fragmentShader);
}

void drawScene() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	float time = glutGet(GLUT_ELAPSED_TIME) / 1500.0f;
	vec2 resolution = vec2(w_update, h_update);
	glUniform1f(loctime, time);
	glUniform2f(locres, resolution.x, resolution.y);
	glUniformMatrix4fv(MatProj, 1, GL_FALSE, value_ptr(Projection));

	//disegnio prato e cielo
	int i;
	for (i = 0; i < 2; i++)
	{
		glUniform1i(lsceltafs, Scena[i].sceltaFS);
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[i].Model));
		glBindVertexArray(Scena[i].VAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, Scena[i].nv);
		glBindVertexArray(0);
	}
	
	//disegno la porta
	glUniform1i(lsceltafs, Scena[2].sceltaFS);
	glBindVertexArray(Scena[2].VAO);
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[2].Model));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
	glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
	for (int i = 0; i < 20; i += 2) {
		glDrawArrays(GL_LINE_STRIP, 12 + i, 2);
	}
	for (int i = 0; i < 20; i += 2) {
		glDrawArrays(GL_LINE_STRIP, 32 + i, 2);
	}

	glBindVertexArray(0);

	//disegno il puntatore-retta
	if (Scena[3].active) {
		glUniform1i(lsceltafs, Scena[3].sceltaFS);
		glBindVertexArray(Scena[3].VAO);
		Scena[3].Model = mat4(1.0);
		//Scena[3].Model = translate(Scena[3].Model, vec3((float)width / 2.0, (float)height / 2.0, 0.0));
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[3].Model));
		glDrawArrays(GL_LINE_STRIP, 0, Scena[3].CP.size());
		glBindVertexArray(0);
	}

	//disegno il pallone
	glBindVertexArray(Scena[4].VAO);
	//matrice di Trasformazione della Palla
	Scena[4].Model = mat4(1.0);
	Scena[4].Model = translate(Scena[4].Model, vec3((float)width / 2.0 + posx_palla, posy_palla, 0.0f));
	Scena[4].Model = scale(Scena[4].Model, vec3(35.0, 35.0, 1.0));
	Scena[4].corner_b = Scena[4].corner_b_obj;
	Scena[4].corner_t = Scena[4].corner_t_obj;
	Scena[4].corner_b = Scena[4].Model * Scena[4].corner_b;
	Scena[4].corner_t = Scena[4].Model * Scena[4].corner_t;
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[4].Model));
	glDrawArrays(GL_TRIANGLE_FAN, 0, (Scena[4].nv - 6));
	//glDrawArrays(GL_LINE_STRIP, Scena[4].nv - 6, 6);
	glBindVertexArray(0);

	//disegno il portiere
	//piede sinistro
	Scena[5].Model = mat4(1.0);
	Scena[5].Model = translate(Scena[5].Model, vec3((float)width/2.0 - 40.0 + dx, (float)height / 3.0, 0.0));
	Scena[5].Model = scale(Scena[5].Model, vec3(25.0, 30.0, 1.0));
	Scena[5].corner_b = Scena[5].corner_b_obj;
	Scena[5].corner_t = Scena[5].corner_t_obj;
	Scena[5].corner_b = Scena[5].Model * Scena[5].corner_b;
	Scena[5].corner_t = Scena[5].Model * Scena[5].corner_t;
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[5].Model));
	glBindVertexArray(Scena[5].VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[5].vertici.size()-6);
	//glDrawArrays(GL_LINE_STRIP, Scena[5].nv - 6, 6);
	glBindVertexArray(0);
	//piede destro
	Scena[6].Model = mat4(1.0);
	Scena[6].Model = translate(Scena[6].Model, vec3((float)width / 2.0 + 40.0 + dx, (float)height / 3.0, 0.0));
	Scena[6].Model = scale(Scena[6].Model, vec3(25.0, 30.0, 1.0));
	Scena[6].Model = rotate(Scena[6].Model, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	Scena[6].corner_b = Scena[6].corner_b_obj;
	Scena[6].corner_t = Scena[6].corner_t_obj;
	Scena[6].corner_b = Scena[6].Model * Scena[6].corner_b;
	Scena[6].corner_t = Scena[6].Model * Scena[6].corner_t;
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[6].Model));
	glBindVertexArray(Scena[6].VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[6].vertici.size()-6);
	//glDrawArrays(GL_LINE_STRIP, Scena[6].nv - 6, 6);
	glBindVertexArray(0);
	//gamba sinistra
	Scena[7].Model = mat4(1.0);
	Scena[7].Model = translate(Scena[7].Model, vec3((float)width / 2.0 + dx, (float)height / 3.0 + 35.0, 0.0));
	Scena[7].Model = scale(Scena[7].Model, vec3(35.0, 34.0, 1.0));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[7].Model));
	glBindVertexArray(Scena[7].VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[7].vertici.size());
	glBindVertexArray(0);
	//gamba destra
	Scena[8].Model = mat4(1.0);
	Scena[8].Model = translate(Scena[8].Model, vec3((float)width / 2.0 + dx, (float)height / 3.0 + 35.0, 0.0));
	Scena[8].Model = scale(Scena[8].Model, vec3(35.0, 35.0, 1.0));
	Scena[8].Model = rotate(Scena[8].Model, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[8].Model));
	glBindVertexArray(Scena[8].VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[8].vertici.size());
	glBindVertexArray(0);
	//corpo sinistro
	Scena[9].Model = mat4(1.0);
	Scena[9].Model = translate(Scena[9].Model, vec3((float)width / 2.0 - 10.0 + dx, (float)height / 2.2, 0.0));
	Scena[9].Model = scale(Scena[9].Model, vec3(52.0, 45.0, 1.0));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[9].Model));
	glBindVertexArray(Scena[9].VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[9].vertici.size());
	glBindVertexArray(0);
	//corpo destro
	Scena[10].Model = mat4(1.0);
	Scena[10].Model = translate(Scena[10].Model, vec3((float)width / 2.0 + 10.0 + dx, (float)height / 2.2, 0.0));
	Scena[10].Model = scale(Scena[10].Model, vec3(52.0, 45.0, 1.0));
	Scena[10].Model = rotate(Scena[10].Model, radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[10].Model));
	glBindVertexArray(Scena[10].VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[10].vertici.size());
	glBindVertexArray(0);
	//mano sinistra
	Scena[11].Model = mat4(1.0);
	Scena[11].Model = translate(Scena[11].Model, vec3((float)width / 2 - 35.0 + dx, (float)height / 2.3, 0.0));
	Scena[11].Model = scale(Scena[11].Model, vec3(12.0, 14.0, 1.0));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[11].Model));
	glBindVertexArray(Scena[11].VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[11].vertici.size() - 6);
	glBindVertexArray(0);
	//mano destra
	Scena[12].Model = mat4(1.0);
	Scena[12].Model = translate(Scena[12].Model, vec3((float)width / 2.0 + 35.0 + dx, (float)height / 2.3, 0.0));
	Scena[12].Model = scale(Scena[12].Model, vec3(12.0, 14.0, 1.0));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[12].Model));
	glBindVertexArray(Scena[12].VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[12].vertici.size() - 6);
	glBindVertexArray(0);
	//testa
	Scena[13].Model = mat4(1.0);
	Scena[13].Model = translate(Scena[13].Model, vec3((float)width / 2.0 + dx, (float)height / 1.8 - 15.0, 0.0));
	Scena[13].Model = scale(Scena[13].Model, vec3(30.0, 32.0, 1.0));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[13].Model));
	glBindVertexArray(Scena[13].VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[13].vertici.size() - 6);
	glBindVertexArray(0);
	//testo lato sx(info sulla partita)
	char src[15];
	sprintf_s(src, "%d", ngol);
	char dest[15] = "GOL = ";
	strcat_s(dest, src);
	RenderText(programId_text, Projection, dest, VAO_Text, VBO_Text, 5.0f, 45.0f, 1.0f, glm::vec3(0.8, 0.4f, 0.0f));
	sprintf_s(src, "%d", potenza);
	char dest1[30] = "LIV POTENZA = ";
	strcat_s(dest1, src);
	RenderText(programId_text, Projection, dest1, VAO_Text, VBO_Text, 5.0f, 5.0f, 1.0f, glm::vec3(0.8, 0.4f, 0.0f));
	RenderText(programId_text, Projection, esitoTiro, VAO_Text, VBO_Text, float(width - 200.0f), 5.0f, 1.0f, glm::vec3(0.8, 0.4f, 0.0f));
	glUseProgram(programId);

	if (checkCollision(Scena[4], Scena[5]) || checkCollision(Scena[4], Scena[6])) {
		collision = true;
		posx = posx_palla;
		posy = posy_palla;
		strcat_s(esitoTiro, "PARATO");
	}
	else
		collision = false;

	glutSwapBuffers();
}

void mouseClick(int button, int state, int x, int y) {
	//uso come centro del sistema il punto in cui è posizionata la palla
	if (!Scena[4].active) {
		esitoTiro[0] = '\0';
		posx = (float)x - (w_window / 2.0);
		posx = (posx * (float)width) / w_update;
		posy = (h_window - (float)y) * (float)height / h_update;
		updatePalla(0);
		glutPostRedisplay();
	}
}

void onMouse(int x, int y) {
	//uso la proporzione x_ottenuto : w_nuovaFinestra = x_equivNellaPrimaFinestra : w_primaFinestra (e analoga in y-h) dato che le proporzioni restano le stesse
	float mousex = ((float)x * (float)width) / w_update; //== x_equivNellaPrimaFinestra
	float mousey = (h_window - (float)y) * (float)height / h_update;
	if (Retta.CP.size() > 1) {
		Retta.CP.pop_back();
	}
	//printf("FINESTRA == %f, %f     SCHERMO == %f, %f\nMOUSE == %d, %d\nPUNTO == %f, %f\nCLICK == %f, %f\n\n", w_window, h_window, w_update, h_update, x, y, mousex, mousey, 
		//(float)x - (w_window / 2.0), (h_window - (float)y) * (float)height / h_update);
	Retta.CP.push_back(vec3(mousex, mousey, 0));
	Retta.active = Scena[3].active;
	crea_VAO_CP(&Retta);
	Scena[3] = Retta;
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	w_window = (float)w;
	h_window = (float)h;
	Projection = ortho(0.0f, (float)width, 0.0f, (float)height);

	float AspectRatio_mondo = (float)(width) / (float)(height); //Rapporto larghezza altezza di tutto ci� che � nel mondo
	 //Se l'aspect ratio del mondo � diversa da quella della finestra devo mappare in modo diverso 
	 //per evitare distorsioni del disegno

	if (AspectRatio_mondo > w / h)   //Se ridimensioniamo la larghezza della Viewport
	{
		w_update = (float)w;
		h_update = w / AspectRatio_mondo;
		glViewport(0, 0, w_update, h_update);
	}
	else {  //Se ridimensioniamo la larghezza della viewport oppure se l'aspect ratio tra la finestra del mondo 
			//e la finestra sullo schermo sono uguali
		w_update = h * AspectRatio_mondo;
		h_update = (float)h;
		glViewport(0, 0, w_update, h_update);

	}
}

void myKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'd':
			if(potenza < 3)
				potenza++;
			break;
		case 'a':
			if (potenza > 1)
				potenza --;
			break;
		case 'f':
			Scena[3].active = !Scena[3].active;
			break;
	}
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(200, 60);
	glutCreateWindow("Penalty Kick");
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(mouseClick);
	glutPassiveMotionFunc(onMouse);
	glutTimerFunc(10, updatePortiere, 0);
	glutReshapeFunc(reshape);

	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();
	//Init VAO per la gestione del disegno
	INIT_VAO_Text();
	Init_Freetype();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutMainLoop();
}