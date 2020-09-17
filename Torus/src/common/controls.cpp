// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // La palabra clave "extern" aqu� es para tener acceso a la variable "window" declarada
							//en tutorialXXX.cpp. Este es un truco para mantener los tutoriales simples. Por favor, evita esto.
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}


// Posici�n inicial : en +Z
glm::vec3 position = glm::vec3( 0, 0, 5 ); 
// Angulo horizontal inicial: hacia -Z
float horizontalAngle = 3.14f;
// Angulo vertical inicial : ninguno
float verticalAngle = 0.0f;
// Campo de visi�n inicial
float initialFoV = 45.0f;

float speed = 3.0f; // 3 unidades / segundo
float mouseSpeed = 0.001f;//0.005f;

void computeMatricesFromInputs(){

	// glfwGetTime se llama s�lo una vez, la primera vez que se llama a esta funci�n
	static double lastTime = glfwGetTime();

	// Calcular la diferencia de tiempo entre el fotograma actual y el �ltimo
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Obtener la posici�n del rat�n
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Restablecer la posici�n del rat�n para el siguiente fotograma
	glfwSetCursorPos(window, 800/2, 600/2);

	// Calcular nueva orientaci�n
	horizontalAngle += mouseSpeed * float(800/2 - xpos );
	verticalAngle   += mouseSpeed * float( 600/2 - ypos );

	// Direcci�n : Coordenadas esf�ricas a la conversi�n de coordenadas cartesianas
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	// Vector derecho
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Subir vector
	glm::vec3 up = glm::cross( right, direction );


	// Avanzar
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Moverse hacia atr�s
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	// Strafe a la izquierda
	if (glfwGetKey( window, GLFW_KEY_LEFT  ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe a la derecha
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Ahora GLFW 3 requiere la configuraci�n de una devoluci�n de llamada para esto. Es un poco demasiado complicado para este tutorial para principiantes, por lo que est� desactivado en su lugar.


	//Matriz de proyecci�n: Campo de visi�n de 45�, relaci�n 4:3, rango de visualizaci�n: 0,1 unidades <-> 100 unidades
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	// Matriz de c�mara
	ViewMatrix  = glm::lookAt(
						position,           // La c�mara est� aqu�
						position+direction, // y mira aqu� : en la misma posici�n, m�s "direcci�n"
						up                  // La cabeza est� hacia arriba (establecida en 0,-1,0 para mirar al rev�s)
	);

	// Para el siguiente fotograma, el "�ltimo tiempo" ser� "ahora"
	lastTime = currentTime;
}
