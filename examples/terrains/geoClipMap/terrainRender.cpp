#include "geometry_clipmappings_terrain.h"
#include "layer.h"
#include "terrainUpdater.h"

#include <gls.h>
#include <shader.h>
#include <camera.h>
#include <helpF.h>
#include <log.h>
#include <texture.h>
#include <resource.h>

void TerrainElement::initGlObj()
{
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	GLuint vbo_;
	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(TerrVertex), &vertices_[0], GL_STATIC_DRAW);

	// Link vertex attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrVertex), (void*)offsetof(TerrVertex, position_));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TerrVertex), (void*)offsetof(TerrVertex, color_));

}

void TerrainElement::destroryGlObj()
{
	if (vao_)
	{
		glDeleteVertexArrays(1, &vao_);
	}
}


void TerrainBlock::initGlObj()
{
	TerrainElement::initGlObj();

	GLuint indices_vbo_;
	glGenBuffers(1, &indices_vbo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(INDICESTYPE), &indices_[0], GL_STATIC_DRAW);

	glBindVertexArray(0);//ÔÚarray_buffer and element_array_buffer bind 0Ç°
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void TerrainElement::render(Shader* shader, const RenderShaderIndex* index, BlockRenderState* state)
{
	shader->setFloat2(index->ulocalOffset_, state->offset_.x, state->offset_.y);
	shader->setFloat(index->uScale_, state->scale_);
	glDrawElements(GL_TRIANGLE_STRIP, indices_.size(), GL_UNSIGNED_INT, NULL);
}

void FixUp::initGlObj()
{
	TerrainElement::initGlObj();

	glGenBuffers(2, indices_vbo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo_[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(INDICESTYPE) / 2, &indices_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo_[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(INDICESTYPE) / 2, &indices_[indices_.size() / 2], GL_STATIC_DRAW);

	glBindVertexArray(0);//ÔÚarray_buffer and element_array_buffer bind 0Ç°
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void FixUp::render(Shader* shader, const RenderShaderIndex* index, BlockRenderState* state, bool vertical)
{
	int idx = (vertical) ? 0 : 1;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo_[idx]);
	shader->setFloat2(index->ulocalOffset_, state->offset_.x, state->offset_.y);
	shader->setFloat(index->uScale_, state->scale_);
	glDrawElements(GL_TRIANGLE_STRIP, indices_.size() / 2, GL_UNSIGNED_INT, NULL);
}

void InteriorTrim::render(Shader* shader, const RenderShaderIndex* index, BlockRenderState* state, bool vertical)
{
	int idx = (vertical) ? 0 : 1;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo_[idx]);
	shader->setFloat2(index->ulocalOffset_, state->offset_.x, state->offset_.y);
	shader->setFloat(index->uScale_, state->scale_);
	(vertical) ? glDrawElements(GL_TRIANGLE_STRIP, num_vertices_indices_, GL_UNSIGNED_INT, NULL) :
		glDrawElements(GL_TRIANGLE_STRIP, indices_.size() - num_vertices_indices_, GL_UNSIGNED_INT, NULL);
}

void InteriorTrim::initGlObj()
{
	TerrainElement::initGlObj();

	glGenBuffers(2, indices_vbo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo_[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_vertices_indices_ * sizeof(INDICESTYPE), &indices_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo_[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (indices_.size() - num_vertices_indices_) * sizeof(INDICESTYPE), &indices_[num_vertices_indices_], GL_STATIC_DRAW);

	glBindVertexArray(0);//ÔÚarray_buffer and element_array_buffer bind 0Ç°
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OutDegenerateTriangles::render(Shader* shader, const RenderShaderIndex* index, BlockRenderState* state, int flag)
{
	int idx = flag;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo_[idx]);
	shader->setFloat2(index->ulocalOffset_, state->offset_.x, state->offset_.y);
	shader->setFloat(index->uScale_, state->scale_);
	glDrawElements(GL_TRIANGLES, indices_.size() / 4, GL_UNSIGNED_INT, NULL);
}

void OutDegenerateTriangles::initGlObj()
{
	TerrainElement::initGlObj();

	glGenBuffers(4, indices_vbo_);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo_[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() / 4 * sizeof(INDICESTYPE), &indices_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo_[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() / 4 * sizeof(INDICESTYPE), &indices_[indices_.size() / 4], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo_[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() / 4 * sizeof(INDICESTYPE), &indices_[indices_.size() / 4 * 2], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo_[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() / 4 * sizeof(INDICESTYPE), &indices_[indices_.size() / 4 * 3], GL_STATIC_DRAW);

	glBindVertexArray(0);//ÔÚarray_buffer and element_array_buffer bind 0Ç°
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	CHECK_GL_ERROR;
}

void FinerEstCenter::initGlObj()
{
	TerrainElement::initGlObj();

	GLuint indices_vbo_;
	glGenBuffers(1, &indices_vbo_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(INDICESTYPE), &indices_[0], GL_STATIC_DRAW);

	glBindVertexArray(0);//ÔÚarray_buffer and element_array_buffer bind 0Ç°
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void FootPrints::renderCenterLevel(Shader* shader, BlockRenderState* state)
{
	glBindVertexArray(center_.vao_);
	center_.render(shader, shader_idx_, state);
	glBindVertexArray(0);
}

void FootPrints::renderBlocks(Shader* shader, BlockRenderState* state, int num)
{
	//block
	glBindVertexArray(block_.vao_);
	for (int i = 0; i < num; i++)
		block_.render(shader, shader_idx_, &state[i]);
	glBindVertexArray(0);
}

void FootPrints::preRenderFixUP()
{
	glBindVertexArray(fixUP_.vao_);
}

void FootPrints::postRenderFixUP()
{
	glBindVertexArray(0);
}

void FootPrints::renderFixUP(Shader* shader, BlockRenderState* state, int num, bool vertical/*=true*/)
{
	for (int i = 0; i < num; i++)
		fixUP_.render(shader, shader_idx_, &state[i], vertical);
}

void FootPrints::preRenderTrim()
{
	glBindVertexArray(trim_.vao_);
}

void FootPrints::postRenderTrim()
{
	glBindVertexArray(0);
}

void FootPrints::renderTrim(Shader* shader, BlockRenderState* state, bool vertical)
{
	trim_.render(shader, shader_idx_, state, vertical);
}

void FootPrints::preRenderDegenerateTriangle()
{
	glBindVertexArray(degenerates_.vao_);
}

void FootPrints::postRenderDegenerateTrianglem()
{
	glBindVertexArray(0);
}

void FootPrints::renderDegenerateTriangle(Shader* shader, BlockRenderState* state, int flag)
{
	degenerates_.render(shader, shader_idx_, state, flag);
}

void ClipmapTerrain::render(const CameraBase* camera)
{
	Shader* currentShader = baseShader_; //normal pass
	currentShader->turnOn();

	Matrixf modeMatrix(1.0);
	currentShader->setMatrix4(state_.uProj_, 1, GL_FALSE, math::value_ptr(camera->getProjectionMatrix()));
	currentShader->setMatrix4(state_.uView_, 1, GL_FALSE, math::value_ptr(camera->getViewMatrix()));
	currentShader->setMatrix4(state_.uMode_, 1, GL_FALSE, math::value_ptr(modeMatrix));
	glActiveTexture(GL_TEXTURE0);
	terrainUpdater_->bindHeightMap(true);

	//glActiveTexture(GL_TEXTURE1);
	//terrainUpdater_->diffuseTexture_->bind();
	layerManager_->render(currentShader, &footprints_);

	terrainUpdater_->bindHeightMap(false);

	currentShader->turnOff();

	CHECK_GL_ERROR;

}

void TerrainClipOutLevel::render(Shader* shader, FootPrints* footPrint)
{
	V2f levelOffet = leftTop();
	shader->setFloat2(manager_->getShaderIndex()->ulevelOffset_, levelOffet.x, levelOffet.y);
	renderBlocks(shader, footPrint);
	renderFixUps(shader, footPrint);
	renderTrims(shader, footPrint);
	renderDegenerateTriangels(shader, footPrint);
}

void TerrainClipInnermostLevel::render(Shader* shader, FootPrints* footPrints)
{
	V2f levelOffet = leftTop();
	shader->setFloat2(manager_->getShaderIndex()->ulevelOffset_, levelOffet.x, levelOffet.y);
	footPrints->renderCenterLevel(shader, &state_);
}

bool ClipmapTerrain::initShader()
{
	std::string code = Shader::loadMultShaderInOneFile("terrain/geometry_clipmappings.glsl");

	baseShader_ = new Shader;
	baseShader_->getShaderFromMultCode(Shader::VERTEX, "base", code);
	baseShader_->getShaderFromMultCode(Shader::FRAGMENT, "base", code);
	baseShader_->linkProgram();
	baseShader_->checkProgram();

	state_.uMode_ = baseShader_->getVariable("model");
	state_.uView_ = baseShader_->getVariable("view");
	state_.uProj_ = baseShader_->getVariable("projection");

	state_.ulevelOffset_ = baseShader_->getVariable("levelOffset");

	state_.ulocalOffset_ = baseShader_->getVariable("localOffset");
	state_.uScale_ = baseShader_->getVariable("scale");

	state_.heightMap_ = baseShader_->getVariable("heightmap");
	state_.diffMap_ = baseShader_->getVariable("diffmap");

	footprints_.setShaderLocations(&state_);
	layerManager_->setShaderLocations(&state_);

	baseShader_->turnOn();
	baseShader_->setInt(state_.heightMap_, 0);
	//baseShader_->setInt(state_.diffMap_, 1);

	baseShader_->turnOff();
	return true;
}
