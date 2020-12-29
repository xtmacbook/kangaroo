#include "comShader.h"
#include "log.h"
Shader_SP createHudShader()
{
	Shader_SP hudShader = new Shader;
	std::string code = Shader::loadMultShaderInOneFile("hudQuad.glsl");

	hudShader->getShaderFromMultCode(Shader::VERTEX, "hud", code);
	hudShader->getShaderFromMultCode(Shader::GEOMETRY, "hud", code);
	hudShader->getShaderFromMultCode(Shader::FRAGMENT, "hud", code);
	hudShader->linkProgram();
	hudShader->checkProgram();

	CHECK_GL_ERROR;

	return hudShader;
}