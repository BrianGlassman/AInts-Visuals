#include <map>

#include "textures.hpp"

std::map<std::string, GLuint> textures = std::map<std::string, GLuint>();

void LoadTexture(std::string key, std::string filename)
{
	textures[key] = LoadTexBMP(("Textures/" + filename).c_str());
}

/// @brief Setup for using textures, based on Ex. 14
void InitTextures()
{
	// OpenGL config
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);

	// Load textures
	LoadTexture("dirt", "dirtGroundTileable.bmp");
}

void BindTexture(std::string key)
{
	GLuint texture = textures[key];
	if (texture == 0)
	{
		Fatal(1, "Texture '%s' not found for binding\n", key.c_str());
	}
	glBindTexture(GL_TEXTURE_2D, textures[key]);
}