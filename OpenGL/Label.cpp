// #include "Label.h"
//
//
// Label::Label(const char* text, glm::vec3 position, glm::vec3 color, Font& font)
// 	: text(text), position(position), color(color), font(font)
// {
// 	m_Shader = new Shader(".\\shaders\\font.vert", ".\\shaders\\font.frag");
// 	load();
// 	m_Shader->Use();
// 	glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
// 	glUniformMatrix4fv(glGetUniformLocation(m_Shader->Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
// 	glEnable(GL_BLEND);
// 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// }
//
// void Label::renderText()
// {
// 	float x = position.x;
// 	float y = position.y;
// 	float scale = 1.0f;
// 	m_Shader->Use();
// 	glUniform3f(glGetUniformLocation(m_Shader->Program, "textColor"), color.x, color.y, color.z);
// 	glActiveTexture(GL_TEXTURE0);
// 	glBindVertexArray(m_VAO);
//
// 	std::string::const_iterator c;
// 	for (c = text.begin(); c != text.end(); c++)
// 	{
// 		Character ch = m_Characters[*c];
//
// 		float xpos = x + ch.Bearing.x * scale;
// 		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
//
// 		float w = ch.Size.x * scale;
// 		float h = ch.Size.y * scale;
// 		float vertices[6][4] = {
// 			{ xpos,     ypos + h,   0.0f, 0.0f },
// 			{ xpos,     ypos,       0.0f, 1.0f },
// 			{ xpos + w, ypos,       1.0f, 1.0f },
//
// 			{ xpos,     ypos + h,   0.0f, 0.0f },
// 			{ xpos + w, ypos,       1.0f, 1.0f },
// 			{ xpos + w, ypos + h,   1.0f, 0.0f }
// 		};
// 		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
// 		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
// 		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
// 		glBindBuffer(GL_ARRAY_BUFFER, 0);
// 		glDrawArrays(GL_TRIANGLES, 0, 6);
// 		x += (ch.Advance >> 6) * scale; // bit shift by 6 to get value in pixels (2^6 = 64)
// 	}
// 	glBindVertexArray(0);
// 	glBindTexture(GL_TEXTURE_2D, 0);
// }
//
// void Label::load()
// {
//
// 	FT_Library ft;
// 	if (FT_Init_FreeType(&ft))
// 	{
// 		std::cout << "ERROR::FREEETYPE::Could not initialize freetype library " << std::endl;
// 	}
//
// 	FT_Face face;
// 	if (FT_New_Face(ft, font.fontPath, 0, &face))
// 	{
// 		std::cout << "ERROR::FREETYPE::Failed to load font";
// 	}
//
// 	FT_Set_Pixel_Sizes(face, 0, font.fontSize);
//
// 	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//
// 	for (unsigned char c = 0; c < 128; c++)
// 	{
// 		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
// 		{
// 			std::cout << "ERROR::FREETYPE:: Failed to load the glyph for " << c << std::endl;
// 			continue;
// 		}
//
// 		unsigned int font_texture;
// 		glGenTextures(1, &font_texture);
// 		glBindTexture(GL_TEXTURE_2D, font_texture);
// 		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
//
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
// 		Character character = {
// 			font_texture,
// 			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
// 			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
// 			face->glyph->advance.x
// 		};
//
// 		m_Characters.insert(std::pair<char, Character>(c, character));
//
// 	}
// 	FT_Done_Face(face);
// 	FT_Done_FreeType(ft);
//
// 	glGenVertexArrays(1, &m_VAO);
// 	glGenBuffers(1, &m_VBO);
// 	glBindVertexArray(m_VAO);
// 	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
// 	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
// 	glEnableVertexAttribArray(0);
// 	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
// 	glBindBuffer(GL_ARRAY_BUFFER, 0);
// 	glBindVertexArray(0);
// }
