#include <glm/glm.hpp> // Include all GLM core / GLSL features vec2, vec3, mat4, radians
#include <glm/ext.hpp> // Include all GLM extensions  perspective, translate, rotate

#include <sstream>


glm::mat4 transform(glm::vec2 const& Orientation, glm::vec3 const& Translate,
	glm::vec3 const& Up)
{
	glm::mat4 Proj = glm::perspective(glm::radians(45.f), 1.33f, 0.1f, 10.f);
	glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.f), Translate);
	glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, Orientation.y, Up);
	glm::mat4 View = glm::rotate(ViewRotateX, Orientation.x, Up);
	glm::mat4 Model = glm::mat4(1.0f);
	return Proj * View * Model;
}

glm::mat4 computeModelViewMatrix(float Translate, glm::vec2 const & Rotate)
{
	glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f)); 
	glm::vec4 c2 = m[3]; //3 col
	/*

		应该是:  [	1	0	0	tx
				    0	1   0   ty
					0	0   1	tz
					0	0	0	1]
	但glm是以列存储的，所以m[3] = (tx,ty,tz,1)	
		   m[a][b] :第a列第b行
	/// // m[0][0] == 1.0f, m[0][1] == 0.0f, m[0][2] == 0.0f, m[0][3] == 0.0f
	/// // m[1][0] == 0.0f, m[1][1] == 1.0f, m[1][2] == 0.0f, m[1][3] == 0.0f
	/// // m[2][0] == 0.0f, m[2][1] == 0.0f, m[2][2] == 1.0f, m[2][3] == 0.0f
	/// // m[3][0] == 1.0f, m[3][1] == 1.0f, m[3][2] == 1.0f, m[3][3] == 1.0f
	*/

	/**
		glm::vec4 * glm::mat4
		(x,y,z,r)  *  mat4 = 

		m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2] + m[0][3] * v[3],
		m[1][0] * v[0] + m[1][1] * v[1] + m[1][2] * v[2] + m[1][3] * v[3],
		m[2][0] * v[0] + m[2][1] * v[1] + m[2][2] * v[2] + m[2][3] * v[3],
		m[3][0] * v[0] + m[3][1] * v[1] + m[3][2] * v[2] + m[3][3] * v[3]);
	*/
	
	glm::mat4 der(2.0f, -2.0f, 1.0f, 1.0f,
		-3.0f, 3.0f, -2.0f, -1.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f);

	/*
		应该是 [						[
				2	-2	1	1		0
				-3	3	-2	-1		1
				0	0	1	0		-5		相乘
				1	0	0	0		0
				]					]	

		但是glm:是					[ 2		-3		0	1
					[0 1 -5 0]		  -2	3		0	0
									  1		-2		1	0
									 1		-1		0	0
									 }
									 */
	glm::vec4 tv(0.0f, 1.0f, -5.0f, 0.0f);
	glm::vec4 r = tv * der;


	glm::mat4 scalem = glm::scale(glm::mat4(1.0),glm::vec3(1.0,2.0,3.0));
	glm::vec4 c1 = scalem[2]; //第三列
	
	glm::mat4 rotatem = glm::rotate(glm::mat4(1.0), (float)3.14159 /4.0f, glm::vec3(0.0, 0.0, 1.0));
	glm::vec4 rc0 = rotatem[0]; //第三列
	glm::vec4 rc1 = rotatem[1]; //第三列

	glm::mat4 result = rotatem * scalem;

	return m;
}

int main()
{
	glm::vec2 t;
	computeModelViewMatrix(1.0, t);
}
