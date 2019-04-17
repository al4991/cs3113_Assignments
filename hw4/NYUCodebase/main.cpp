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
#include <string>
#include <math.h>
using namespace std;
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6


float accumulator = 0.0f;	
const int runAnimation[] = { 17, 23 };
const int numFrames = 2;
float animationElapsed = 0.0f;
float framesPerSecond = 60.0f;
int currentIndex = 0;

SDL_Window* displayWindow;
enum EntityType { ENTITY_PLAYER, ENTITY_BLOCK, ENTITY_COIN };

float lerp(float v0, float v1, float t) {
	return (1.0 - t)*v0 + t * v1;
}


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
			-0.5f * size * aspect, -0.5f * size,
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

	void DrawSpriteSheetSprite(ShaderProgram &program, int index, int spriteCountX, int spriteCountY) {
		float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
		float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
		float spriteWidth = 1.0 / (float)spriteCountX;
		float spriteHeight = 1.0 / (float)spriteCountY;
		GLfloat texCoords[] = {
		u, v + spriteHeight,
		u + spriteWidth, v,
		u, v,
		u + spriteWidth, v,
		u, v + spriteHeight,
		u + spriteWidth, v + spriteHeight
		};
		float aspect = width / height;
		float vertices[] = {
			-0.5f * size , -0.5f * size,
			0.5f * size, 0.5f * size,
			-0.5f * size , 0.5f * size,
			0.5f * size, 0.5f * size,
			-0.5f * size, -0.5f * size ,
			0.5f * size , -0.5f * size
		};

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
	}
};

class Entity {

public:
	float x = 0.0f;
	float y = 0.0f;
	float width;
	float height;
	float x_vel;
	float y_vel;
	float x_acc; 
	float y_acc;
	float x_fric;
	float y_fric;

	bool isStatic;
	EntityType entityType;

	bool collidedTop;
	bool collidedBottom;
	bool collidedLeft;
	bool collidedRight;

	SheetSprite sprite;
	std::vector<int> spriteData; 

	Entity(float width_, float height_, float x_ = 0.0f, float y_ = 0.0f) {
		height = height_ / 2;
		width = width_ / 2;
		x = x_;
		y = y_;
		x_vel = 0.0f;
		y_vel = 0.0f;
		x_acc = 0.0f; 
		y_acc = 0.0f; 
		x_fric = 0.0f; 
		y_fric = 0.0f; 

		collidedTop = false; 
		collidedBottom = false; 
		collidedLeft = false; 
		collidedRight = false; 

	}

	void Render(ShaderProgram& p) {
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, 0.0f));
		p.SetModelMatrix(modelMatrix);

		//if (isStatic){
		//	sprite.draw(p);
		//}
		//else {
			sprite.DrawSpriteSheetSprite(p, spriteData[0], spriteData[1], spriteData[2]);

		/*}*/
	}

	void resolveCollisionX(Entity& entity) {
		float x_dist = x - entity.x;
		float x_pen = fabs(x_dist - width - entity.width);
		if (entity.x > x) {
			x -= x_pen + 0.1f;
		}
		else {
			x += x_pen + 0.1f; 
		}
	}

	void resolveCollisionY(Entity& entity) {
		float y_dist = y - entity.y;
		float y_pen = fabs(y_dist - height - entity.height);
		if (entity.y > y) {
			y -= y_pen + 0.03f;
		}
		else {
			y += y_pen + 0.03f; 
		}
	}

	bool CollidesWith(Entity& entity) {
		float x_dist = fabs(x - entity.x) - (width + entity.width);
		float y_dist = fabs(y - entity.y) - (height + entity.height);
		return (x_dist <= 0 && y_dist <= 0);
	}

	void Update(float elapsed) {
		if (!isStatic) {
			x_vel = lerp(x_vel, 0.0f, elapsed * x_fric);
			y_vel = lerp(y_vel, 0.0f, elapsed * y_fric);

			x_vel +=  x_acc * elapsed;
			y_vel +=  y_acc * elapsed;

			if (entityType == ENTITY_PLAYER && collidedBottom == false) {
				y_vel += -9.8f * elapsed;
			}

			x += x_vel * elapsed;
			y += y_vel * elapsed;

		}
	}
};

bool offScreen(Entity ent) {
	bool offX = true;
	bool offY = true;
	if (-1.0f < ent.y < 1.0f) {
		offY = false;
	}
	if (-1.777f < ent.x < 1.777f) {
		offX = false;
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
	program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	GLuint spriteSheetTexture = LoadTexture(RESOURCE_FOLDER"cuties.png");
	GLuint fontTexture = LoadTexture(RESOURCE_FOLDER"font1.png");
	GLuint floorTexture = LoadTexture(RESOURCE_FOLDER"JnRTiles.png");
	glClearColor(0.8f, 1.0f, 1.0f, 1.0f);

	float lastFrameTicks = 0.0f;

	glm::mat4 projectionMatrix = glm::mat4(1.0f);
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-1.777f, 1.777f, -1.0f, 1.0f, -1.0f, 1.0f);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glUseProgram(program.programID);

	// Creating objects 

	Entity player = Entity(0.1f, 0.1f, 0.0f, 0.5f);
	player.sprite = SheetSprite(spriteSheetTexture, (float)(((int)17) % 6) / (float)6, (float)(((int)17) / 6) / (float)8, 1.0f / (float)6, 1.0f / (float)8, 0.2f);
	player.entityType = ENTITY_PLAYER;
	player.isStatic = false;
	player.x_fric = 2.5f;
	player.spriteData = { 17, 6, 8 };

	/*
		Entity floor = Entity(1.77f, 0.5f, 0.0f, -0.9f);
		floor.sprite = SheetSprite(floorTexture, (float)(((int)15) % 18) / (float)18, (float)(((int)15) / 18) / (float)1, 1.0f / (float)18, 1.0f / (float)1, 1.0f);
		floor.entityType = ENTITY_BLOCK;
		floor.isStatic = true;
	*/
	std::vector<Entity> floor;
	float start_x = -1.67f;
	float start_y = -0.7f;
	float u = (float)(((int)15) % 18) / (float)18;
	float v = (float)(((int)15) / 18) / (float)1;
	float width = 1.0f / (float)18;
	float height = 1.0f / (float)1;
	int blocks[60] = {
		14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
		15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15
	};
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 30; j++) {
			Entity newBlock = Entity(0.2f, 0.2f, (start_x + 0.2f * j), start_y - 0.2f * i);
			newBlock.isStatic = true;
			newBlock.entityType = ENTITY_BLOCK;
			newBlock.sprite = SheetSprite(floorTexture, u, v, width, height, 0.2f);
			newBlock.spriteData = { blocks[(i * 30) + j], 18, 1 };
			floor.push_back(newBlock);
		}
	}



	SDL_Event event;
	bool done = false;
	while (!done) {
		glClear(GL_COLOR_BUFFER_BIT);
		viewMatrix = glm::mat4(1.0f); 
		viewMatrix = glm::translate(viewMatrix, glm::vec3(std::min(-player.x, 0.0f), std::min(-player.y, 0.0f), 0.0f));
		program.SetProjectionMatrix(projectionMatrix);
		program.SetViewMatrix(viewMatrix);
		glUseProgram(program.programID);

		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
					if (player.collidedBottom) {
						player.y_vel += 5.0f;
						player.collidedBottom = false;
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_S) {
					
					player.y_vel -= 5.0f; 
				}

			}
		}
		if (keys[SDL_SCANCODE_A]) {
			if (player.x > -1.67f) {
				player.x_acc = -1.0f;

			}
		}
		else if (keys[SDL_SCANCODE_D]) {
			if (player.x < 1.67f) {
				player.x_acc = 1.0f;
			}
		}
		
		elapsed += accumulator;
		if (elapsed < FIXED_TIMESTEP) {
			accumulator = elapsed; 
		} 
						
		
		while (elapsed >= FIXED_TIMESTEP) {
			// Update
			player.Update(FIXED_TIMESTEP); 
			player.x_acc = 0.0f;
			for (Entity i : floor)
			if (player.CollidesWith(i)) {
				player.resolveCollisionY(i); 
				player.collidedBottom = true;
			}

			elapsed -= FIXED_TIMESTEP;
		}
		accumulator = elapsed; 


		for (Entity i : floor) {
			i.Render(program); 
		}
		player.Render(program);

		SDL_GL_SwapWindow(displayWindow);
	}


	SDL_Quit();
	return 0;
}