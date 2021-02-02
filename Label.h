#pragma once
#include <iostream>
#include <map>
#include <string>
// GLEW
#include <GL/glew.h>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
struct Character
{
	unsigned int	TextureID;
	glm::ivec2		Size;
	glm::ivec2		Bearing;
	unsigned int	Advance;
};

struct Font
{
	const char*		fontPath;
	unsigned int	fontSize;

	Font(const char* path, unsigned int size)
	{
		this->fontPath = path;
		this->fontSize = size;
	}
};

class Label
{
public:
	std::string					text;
	glm::vec3					position;
	glm::vec3					color;
	Font						font;
private:
	std::map<char, Character>	m_Characters;
	Shader*						m_Shader;
	unsigned int				m_VAO;
	unsigned int				m_VBO;
public:
	Label(const char* text, glm::vec3 position, glm::vec3 color, Font& font);

	void renderText();
private:
	void load();
};