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
#include <vector> 
#include <iostream> 
#include <algorithm>
#include <math.h>
using namespace std;


SDL_Window* displayWindow;


class SheetSprite {

public:
	float size;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;

	SheetSprite() {};
	SheetSprite(unsigned int textureID_, float u_, float v_, float width_, float height_, float size_) {
		size = size_;
		textureID = textureID_;
		u = u_;
		v = v_;
		width = width_;
		height = height_;
	};

	void draw(ShaderProgram &program) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, textureID);
		GLfloat texCoords[] = {
			u, v + height,
			u + width, v,
			u, v,
			u + width, v,
			u, v + height,
			u + width, v + height
		};
		float aspect = width / height;
		float vertices[] = {
			-0.5f * size * aspect, -0.5f * size,
			0.5f * size * aspect, 0.5f * size,
			-0.5f * size * aspect, 0.5f * size,
			0.5f * size * aspect, 0.5f * size,
			-0.5f * size * aspect, -0.5f * size ,
			0.5f * size * aspect, -0.5f * size
		};


		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
	};
};


class Entity {

public:
	float x = 0.0f;
	float y = 0.0f;
	float width;
	float height;
	float x_vel;
	float y_vel;
	float timeAlive = 0.0f;

	SheetSprite sprite;

	Entity(float width_, float height_, float x_ = 0.0f, float y_ = 0.0f) {
		height = height_ / 2;
		width = width_ / 2;
		x = x_;
		y = y_;
		x_vel = 0.0f;
		y_vel = 0.0f;

	}

	void draw(ShaderProgram& p) {
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, 0.0f)); 
		p.SetModelMatrix(modelMatrix); 
		sprite.draw(p);
	}

	void move(float elapsed, float xshift, float yshift) {
		x += (elapsed * x_vel);
		y += (elapsed * y_vel);
	}
};

bool offScreen(Entity ent) {
	bool offX = true;
	bool offY = true;
	if (-1.0f < ent.x < 1.0f) {
		offX = false;
	}
	if (-1.777f < ent.y < 1.777f) {
		offY = false;
	}
	return offX && offY;
}

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

bool collisionCheck(Entity& obj1, Entity& obj2) {
	float x_distance = fabs(obj1.x - obj2.x) - ((obj1.width + obj2.width));
	float y_distance = fabs(obj1.y - obj2.y) - ((obj1.height + obj2.height));
	if (x_distance <= 0 && y_distance <= 0) {
		return true;
	}
	return false;
}

bool shouldRemove(Entity bullet) {
	if (bullet.timeAlive > 2.0f) {
		return true;
	}
	else {
		return false;
	}
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("yote", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 540, 960, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 540, 960);

	ShaderProgram program;
	program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	GLuint spriteSheetTexture = LoadTexture(RESOURCE_FOLDER"sheet.png");


	float lastFrameTicks = 0.0f;


	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-1.0f, 1.0f, -1.777f, 1.777f, -1.0f, 1.0f);
	glUseProgram(program.programID);


	// Creating objects 

	Entity player = Entity(0.1f, 0.1f, 0.0f, -1.6f);
	player.sprite = SheetSprite(spriteSheetTexture, 211.0f / 1024.0f, 941.0f / 1024.0f, 99.0f / 1024.0f, 75.0f / 1024.0f, .2f);

	std::vector<Entity> enemies;
	for (int i = 0; i < 15; i++) {
		float x = (-0.8f + (float)((i % 5) * 0.4f));
		float y = (1.6f - (float)(floor(i / 5) * 0.3f));
		Entity newEnt = Entity(0.2f, 0.2f, x, y);
		newEnt.sprite = SheetSprite(spriteSheetTexture, 423.0f / 1024.0f, 728.0f / 1024.0f, 93.0f / 1024.0f, 84.0f / 1024.0f, 0.2f);
		enemies.push_back(newEnt);
	}
	std::vector<Entity> bullets;


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
			else if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
					Entity newBullet = Entity(0.05f, 0.08f, player.x, player.y + 0.08f);
					newBullet.y_vel = 1.9f;
					newBullet.sprite = SheetSprite(spriteSheetTexture, 778.0f / 1024.0f, 557.0f / 1024.0f, 31.0f / 1024.0f, 30.0f / 1024.0f, 0.05f);
					bullets.push_back(newBullet);
				}
			}
		}
		if (keys[SDL_SCANCODE_A]) {
			if (player.x > -0.95f) {
				player.x -= (elapsed * .7f);

			}
		}
		else if (keys[SDL_SCANCODE_D]) {
			if (player.x < 0.95f) {
				player.x += (elapsed * .7f);

			}
		}
		glClear(GL_COLOR_BUFFER_BIT);

		program.SetProjectionMatrix(projectionMatrix);
		program.SetViewMatrix(viewMatrix);

		glUseProgram(program.programID);


		player.draw(program);
		for (Entity i : enemies) {
			i.draw(program);
		}

		bullets.erase(std::remove_if(bullets.begin(), bullets.end(), shouldRemove), bullets.end());

		for (int i = 0; i < bullets.size(); i++) {
			bullets[i].timeAlive += elapsed;
			bullets[i].move(elapsed, 0.0f, 0.7f);
			bullets[i].draw(program);
			for (int j = 0; j < enemies.size(); j++) {
				if (collisionCheck(bullets[i], enemies[j])) {
					bullets[i].x += 2000.0f;
					enemies[j].x -= 2000.0f;

				}
			}
		}
		bullets.erase(std::remove_if(bullets.begin(), bullets.end(), offScreen), bullets.end());
		enemies.erase(std::remove_if(enemies.begin(), enemies.end(), offScreen), enemies.end());


		// End of the line pal. Nothing after this point. 
		SDL_GL_SwapWindow(displayWindow);
	}




	SDL_Quit();
	return 0;
}
