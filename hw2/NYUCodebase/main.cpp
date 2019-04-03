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
#include <math.h>

SDL_Window* displayWindow;

class Object {

public: 
	float x_pos = 0.0f; 
	float y_pos = 0.0f;
	float half_width; 
	float half_height; 
	float x_dir; 
	float y_dir; 

	Object(float width, float height, float x=0.0f, float y=0.0f) {
		half_height = height / 2; 
		half_width = width / 2; 
		x_pos = x;
		y_pos = y;
		x_dir = 0.0f;
		y_dir = 0.0f; 
	}

	void draw(ShaderProgram& p) {
		glm::mat4 modelMatrix = glm::mat4(1.0f); 
		modelMatrix = glm::translate(modelMatrix, glm::vec3(x_pos, y_pos, 0.0f)); 
		p.SetModelMatrix(modelMatrix); 

		float vertices[] = {
			-half_width, -half_height, 
			half_width, -half_height, 
			half_width, half_height, 
			-half_width, -half_height, 
			half_width, half_height, 
			-half_width, half_height, 
		};

		glVertexAttribPointer(p.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(p.positionAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(p.positionAttribute);
	}

	
	void setRandomDir() {
		x_dir = -1.0f; 
		y_dir = 1.0f; 
	}

	void move(float elapsed) {
		x_pos += (elapsed * 0.5f * x_dir); 
		y_pos += (elapsed * 0.5f * y_dir); 
	}

	/* 
	Returns an int to represent whether the object is off screen, and wihch side it is on. Possible returns listed below
		-1 : not offscreen
		0 : offscreen, and went off the left side (computer wins)
		1 : offscreen and went off the right side (player wins)
	*/
	int checkOffscreen(float x, float y) {
		bool offX = false;
		bool offY = false;
		if (x_pos > x || x_pos < (x * -1.0f)) {
			offX = true;
		}
		if (y_pos > y || y_pos < (y * -1.0f)) {
			offY = true;
		}
		
		if (!offX && !offY) {
			return -1;
		}
		else {
			if (x_pos > 0.0f) { return 1;}
			else { return 0;}
		}
	}
};

bool collisionCheck(Object& obj1, Object& obj2) {
	float x_distance = fabs(obj1.x_pos - obj2.x_pos) - ((obj1.half_width + obj2.half_width));
	float y_distance = fabs(obj1.y_pos - obj2.y_pos) - ((obj1.half_height + obj2.half_height));
	if (x_distance <= 0 && y_distance <= 0) {
		return true;
	}
	return false;
}

void drawMidline(ShaderProgram& p) {
	Object block = Object(0.07f, 0.07f, 0.0f, 0.95f);
	for (int i = 0; i < 15; i++) {
		block.draw(p); 
		block.y_pos -= 0.135f;
	}
}



int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("yote", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 960, 540, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 960, 540);
	ShaderProgram program;
	program.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");

	float lastFrameTicks = 0.0f;

	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::mat4(1.0f);

	projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f);
	glUseProgram(program.programID);

	int offScreenCheck = -1; 
	// Creating objects 
	Object paddle1 = Object(0.07f, 0.25f, -1.465f);
	Object paddle2 = Object(0.07f, 0.25f, 1.465f);
	paddle2.y_dir = 1.0f; 

	Object border1 = Object(3.0f, 0.05f, 0.0f, 0.975f);
	Object border2 = Object(3.0f, 0.05f, 0.0f, -0.975f);

	Object ball = Object(0.075f, 0.075f);
	ball.setRandomDir(); 

	SDL_Event event;
	bool done = false;
	while (!done) {

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		if (keys[SDL_SCANCODE_W]) {
			if (!collisionCheck(paddle1, border1)) {
				paddle1.y_pos += (elapsed * .7);

			}
		}
		else if (keys[SDL_SCANCODE_S]) {
			
			if (!collisionCheck(paddle1, border2)) {
				paddle1.y_pos -= (elapsed * .7);

			}
		}
		/*if (keys[SDL_SCANCODE_UP]) {
			if (!collisionCheck(paddle2, border1)) {
				paddle2.y_pos += (elapsed * .7);

			}
		}
		else if (keys[SDL_SCANCODE_DOWN]) {

			if (!collisionCheck(paddle2, border2)) {
				paddle2.y_pos -= (elapsed * .7);

			}
		}*/
        glClear(GL_COLOR_BUFFER_BIT);

		program.SetProjectionMatrix(projectionMatrix);
		program.SetViewMatrix(viewMatrix);
		
		glUseProgram(program.programID); 

		// Checking for offscreens
		offScreenCheck = ball.checkOffscreen(1.777f, 1.0f);
		if (offScreenCheck != -1) {
			if (offScreenCheck == 0) {
				ball.x_pos = 0; 
				ball.y_pos = 0; 
				ball.x_dir = 1.0f; 
			}
			if (offScreenCheck == 1) {
				ball.x_pos = 0;
				ball.y_pos = 0;
				ball.x_dir = -1.0f;
			}
		}



		// Ball 
		if (collisionCheck(ball, border1) || collisionCheck(ball, border2)) {
			ball.y_dir *= -1.0f;
		}
		if (collisionCheck(ball, paddle1) || collisionCheck(ball, paddle2)) {
			ball.x_dir *= -1.0f;
		}
		ball.move(elapsed);
		ball.draw(program);

		// Borders
		border1.draw(program); 
		border2.draw(program); 

		// Paddles 
		paddle1.draw(program); 

	
		if (collisionCheck(paddle2, border1)) {
			paddle2.y_dir = -1.0f;
		}
		else if (collisionCheck(paddle2, border2)) {
			paddle2.y_dir = 1.0f;
		}
		else {
			if (ball.y_pos >= paddle2.y_pos) {
				paddle2.y_dir = 1.0f; 
			}
			if (ball.y_pos < paddle2.y_pos) {
				paddle2.y_dir = -1.0f;
			}
		}
		
	
		paddle2.y_pos += ((elapsed * 0.7f) * paddle2.y_dir); 

		paddle2.draw(program); 


		// Midline
		drawMidline(program); 
		
	
		


		// End of the line pal. Nothing after this point. 
		SDL_GL_SwapWindow(displayWindow);
    }


	
    
    SDL_Quit();
    return 0;
}
