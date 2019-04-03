#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL_opengl.h>
#include <SDL_image.h>

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ShaderProgram.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

SDL_Window* displayWindow;

GLuint LoadTexture(const char *filePath) {
	int w, h, comp;
	unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);

	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";

	}

	GLuint retTexture;
	glGenTextures(1, &retTexture);
	glBindTexture(GL_TEXTURE_2D, retTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(image);
	return retTexture;
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 540, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

	glViewport(0, 0, 960, 540);
	ShaderProgram program; 
	program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl"); 

	ShaderProgram program2; 
	program2.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
	
	glClearColor(0.8f, 1.0f, 1.0f, 1.0f); 
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint grookeyTexture = LoadTexture(RESOURCE_FOLDER"grookey.png"); 
	GLuint sobbleTexture = LoadTexture(RESOURCE_FOLDER"sobble.png"); 
	GLuint scorbunnyTexture = LoadTexture(RESOURCE_FOLDER"scorbunny.png"); 
	GLuint sunTexture = LoadTexture(RESOURCE_FOLDER"sun.png");


	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::mat4(1.0f);

	projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f);

	glUseProgram(program.programID);


	
    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);


		program.SetModelMatrix(modelMatrix);
		program.SetProjectionMatrix(projectionMatrix);
		program.SetViewMatrix(viewMatrix);

		program2.SetModelMatrix(modelMatrix);
		program2.SetProjectionMatrix(projectionMatrix);
		program2.SetViewMatrix(viewMatrix);


		glUseProgram(program2.programID);

		//Drawing 1
		program2.SetColor(0.219f, 0.4f, 0.0f, 1.0f);

		float vertices4[] = { -2.0, -1.5,
			2.0, -1.5,
			2.0, -.2,
			-2.0, -1.5,
			2.0, -0.2,
			-2.0, -0.2
		};
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices4);
		glEnableVertexAttribArray(program.positionAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		
		
	
		glUseProgram(program.programID);

		// Drawing 2
		glBindTexture(GL_TEXTURE_2D, grookeyTexture);
		float vertices2[] = { -1.2, -0.55, 
			-0.45, -0.55, 
			-0.45, 0.2, 
			-1.2, -0.55, 
			-0.45, 0.2, 
			-1.2, 0.2 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
		glEnableVertexAttribArray(program.positionAttribute);

		float texCoords2[] = { 0.0, 1.0, 
			1.0, 1.0, 
			1.0, 0.0, 
			0.0, 1.0, 
			1.0, 0.0, 
			0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
		
		

		// Drawing 3 
		glBindTexture(GL_TEXTURE_2D, scorbunnyTexture);
		float vertices3[] = { -1.25, -0.65, 
			1.25, -0.65,
			1.25, 0.65, 
			-1.25, -0.65, 
			1.25, 0.65,
			-1.25, 0.65 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
		glEnableVertexAttribArray(program.positionAttribute);

		float texCoords3[] = { 0.0, 1.0, 
			1.0, 1.0, 
			1.0, 0.0, 
			0.0, 1.0, 
			1.0, 0.0, 
			0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords3);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
		


		// Drawing 4
		glBindTexture(GL_TEXTURE_2D, sobbleTexture);

		float vertices1[] = { 0.5, -0.5,
			1.25, -0.5,
			1.25, 0.25,
			0.5, -0.5,
			1.25, 0.25,
			0.5, 0.25 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices1);
		glEnableVertexAttribArray(program.positionAttribute);

		float texCoords1[] = { 0.0, 1.0,
			1.0, 1.0,
			1.0, 0.0,
			0.0, 1.0,
			1.0, 0.0,
			0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords1);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);



		// Drawing 5 
		glBindTexture(GL_TEXTURE_2D, sunTexture);

		float vertices5[] = { -2.1, 0.25,
			-1.0, 0.25,
			-1.0, 1.35,
			-2.1, 0.25,
			-1.0, 1.35,
			-2.1, 1.35};
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices5);
		glEnableVertexAttribArray(program.positionAttribute);
		
		float texCoords5[] = { 0.0, 1.0,
			1.0, 1.0,
			1.0, 0.0,
			0.0, 1.0,
			1.0, 0.0,
			0.0, 0.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords5);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		

        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
