// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#define GLEW_STATIC	//ELIMINA ERRORES DE COMPILACION
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow *window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// Include librerias
#include "common/shader.hpp"
#include "common/controls.hpp"
#include "common/objloader.hpp"

#include "Torus.h"
int main(void) {
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf( stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Abra una ventana y cree su contexto OpenGL
	window = glfwCreateWindow(800, 600, "Esfera", NULL, NULL);
	if (window == NULL) {
		fprintf( stderr,
				"No se pudo abrir la ventana GLFW. Si tiene una GPU Intel, no son compatibles con 3.3. Prueba la versión 2.1 de los tutoriales.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Necesario para el perfil del núcleo
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "No se pudo inicializar GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Asegúrese de que podamos capturar la tecla de escape que se presiona a continuación
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Oculte el mouse y habilite un movimiento ilimitado
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Coloca el mouse en el centro de la pantalla
	glfwPollEvents();
	glfwSetCursorPos(window, 800 / 2, 600 / 2);

	// Fondo azul oscuro
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);



	// Habilitar prueba de profundidad
	glEnable(GL_DEPTH_TEST);//necesario
	// Acepta fragmento si está más cerca de la cámara que el anterior.
	glDepthFunc(GL_LESS);

	// Elimina triángulos que lo normal no es hacia la cámara
	glEnable(GL_CULL_FACE);


	// Cree y compile nuestro programa GLSL a partir de los sombreadores
	GLuint programID = LoadShaders("src/VertexShader.vertexshader",
			"src/FragmentShader.fragmentshader");

	// Obtenga un identificador para nuestro uniforme "MVP"
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	GLfloat colorID = glGetUniformLocation(programID, "colorIn");

	Torus myTorus(0.5f, 0.2f, 15);

	std::vector<int> ind = myTorus.getIndices();
	std::vector<glm::vec3> vert = myTorus.getVertices();
	//std::vector<glm::vec2> tex = myTorus.getTexCoords();
	//std::vector<glm::vec3> norm = myTorus.getNormals();
	std::vector<float> pvalues;
	//std::vector<float> tvalues;
	//std::vector<float> nvalues;
	int numVertices = myTorus.getNumVertices();
	for (int i = 0; i < numVertices; i++) {
		pvalues.push_back(vert[i].x);
		pvalues.push_back(vert[i].y);
		pvalues.push_back(vert[i].z);
		//tvalues.push_back(tex[i].s);
		//tvalues.push_back(tex[i].t);
		//nvalues.push_back(norm[i].x);
		//nvalues.push_back(norm[i].y);
		//nvalues.push_back(norm[i].z);
	}

	GLuint vao[1];
	GLuint vbo[3];

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);

	glGenBuffers(4, vbo); // generate VBOs as before, plus one for indices


		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); // vertex positions
		glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0],
				GL_STATIC_DRAW);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]); // indices
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0],
				GL_STATIC_DRAW);
	/*

	glGenBuffers(4, vbo); // generate VBOs as before, plus one for indices
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); // vertex positions

	glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0],
			GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]); // texture coordinates
	glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0],
			GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]); // normal vectors
	glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0],
			GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]); // indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0],
			GL_STATIC_DRAW);
*/
	do {

		//Limpiar la pantalla
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Usa nuestro sombreador
		glUseProgram(programID);

		// Calcule la matriz MVP desde la entrada del teclado y el mouse
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Envíe nuestra transformación al sombreador enlazado actualmente,
		// con el uniforme "MVP"
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1er búfer de atributo: vértices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glVertexAttribPointer(
				0,                  // attribute
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*) 0            // array buffer offset
				);


		glProgramUniform1f(programID, colorID, 0.0f);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
		glDrawElements(GL_TRIANGLES, myTorus.getNumIndices(), GL_UNSIGNED_INT,0);



		glProgramUniform1f(programID, colorID, 1.0f);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
		glDrawElements(GL_LINE_LOOP, myTorus.getNumIndices(), GL_UNSIGNED_INT,	0);


		glDisableVertexAttribArray(0);

		// Intercambiar búferes
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Compruebe si se presionó la tecla ESC o si la ventana estaba cerrada
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
			&& glfwWindowShouldClose(window) == 0);

	// Limpieza de VBO y sombreador
	glDeleteBuffers(1, vbo);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, vao);

	// Cierre la ventana de OpenGL y finalice GLFW
	glfwTerminate();

	return 0;
}

