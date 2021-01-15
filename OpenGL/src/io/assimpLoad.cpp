#include "meshLoad.h"
#include "mesh.h"
#include "texture.h"
#include "log.h"
#include "geometry.h"

#include <sstream>

#ifdef ENGINE_ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#endif // ENGINE_ASSIMP


#ifdef ENGINE_ASSIMP
void loadMaterialTextures(aiMaterial* mat, aiTextureType type, TEXTURE_TYPE t, Mesh_SP& m, const std::string& dir, std::vector<Texture_Sp>& ts)
{
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		{   // if texture hasn't been loaded already, load it
			bool skip = false;
			for (unsigned int j = 0; j < ts.size(); j++)
			{
				if (std::strcmp(ts[j]->fileName_.c_str(), (dir + "/" + str.C_Str()).c_str()) == 0)
				{
					skip = true;
					break;
				}
			}

			if (!skip)
			{
				ts.push_back(new Texture((dir + "/" + str.C_Str()).c_str()));
				m->setTextureIndices(ts.size() - 1);
				ts[ts.size() - 1]->setType(t);
			}
		}
	}
}

Mesh_SP processMesh(aiMesh* aimesh, const aiScene* scene, std::vector<Texture_Sp>& textures, const std::string& dir)
{
	// Walk through each of the mesh's vertices

	Mesh_SP mesh;

	if (aimesh->mNormals &&
		aimesh->mTextureCoords[0] &&
		aimesh->mTangents)
	{
		mesh = new Mesh(VERTEX_POINTS_NORMAL_TEXTURE_TBN);
		mesh->createMesh(aimesh->mNumVertices);
		for (unsigned int i = 0; i < aimesh->mNumVertices; i++)
		{
			Vertex_PNTTBN vertex(
				V3f(aimesh->mVertices[i].x, aimesh->mVertices[i].y, aimesh->mVertices[i].z),
				V3f(aimesh->mNormals[i].x, aimesh->mNormals[i].y, aimesh->mNormals[i].z),
				V2f(aimesh->mTextureCoords[0][i].x, aimesh->mTextureCoords[0][i].y),
				V3f(aimesh->mTangents[i].x, aimesh->mTangents[i].y, aimesh->mTangents[i].z),
				V3f(aimesh->mBitangents[i].x, aimesh->mBitangents[i].y, aimesh->mBitangents[i].z)
			);
			mesh->addVertex(&vertex);
		}
	}
	else if (aimesh->mNormals &&
		aimesh->mTextureCoords[0])
	{
		mesh = new Mesh(VERTEX_POINTS_NORMAL_TEXTURE);
		mesh->createMesh(aimesh->mNumVertices);

		for (unsigned int i = 0; i < aimesh->mNumVertices; i++)
		{
			Vertex_PNT vertex(
				V3f(aimesh->mVertices[i].x, aimesh->mVertices[i].y, aimesh->mVertices[i].z),
				V3f(aimesh->mNormals[i].x, aimesh->mNormals[i].y, aimesh->mNormals[i].z),
				V2f(aimesh->mTextureCoords[0][i].x, aimesh->mTextureCoords[0][i].y)
			);
			mesh->addVertex(&vertex);
		}
	}
	else if (aimesh->mTextureCoords[0])
	{
		mesh = new Mesh(VERTEX_POINTS_TEXTURE);
		mesh->createMesh(aimesh->mNumVertices);
		for (unsigned int i = 0; i < aimesh->mNumVertices; i++)
		{
			Vertex_PT vertex(
				V3f(aimesh->mVertices[i].x, aimesh->mVertices[i].y, aimesh->mVertices[i].z),
				V2f(aimesh->mTextureCoords[0][i].x, aimesh->mTextureCoords[0][i].y)
			);
			mesh->addVertex(&vertex);
		}
	}
	else if (aimesh->mNormals)
	{
		mesh = new Mesh(VERTEX_POINTS_NORMAL);
		mesh->createMesh(aimesh->mNumVertices);

		for (unsigned int i = 0; i < aimesh->mNumVertices; i++)
		{
			Vertex_PN vertex(
				V3f(aimesh->mVertices[i].x, aimesh->mVertices[i].y, aimesh->mVertices[i].z),
				V3f(aimesh->mNormals[i].x, aimesh->mNormals[i].y, aimesh->mNormals[i].z)
			);
			mesh->addVertex(&vertex);
		}
	}
	else
	{
		PRINT_ERROR("load obj and fail to parse the vertex type! \n");
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < aimesh->mNumFaces; i++)
	{
		aiFace face = aimesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			mesh->addIndices(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[aimesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	loadMaterialTextures(material, aiTextureType_DIFFUSE, T_DIFFUSE, mesh, dir, textures);
	loadMaterialTextures(material, aiTextureType_SPECULAR, T_SPECULAR, mesh, dir, textures);
	loadMaterialTextures(material, aiTextureType_HEIGHT, T_NORMAL, mesh, dir, textures);
	loadMaterialTextures(material, aiTextureType_AMBIENT, T_HEIGHT, mesh, dir, textures);

	mesh->computeBox();

	return mesh;
}

void processNode(aiNode* node, const aiScene* scene, std::vector<Mesh_SP >&meshs,
	std::vector<unsigned int>&primitive_models, IO::LModelInfo&loadInfo, const std::string& dir)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* aimesh = scene->mMeshes[node->mMeshes[i]];
		Mesh_SP mesh = processMesh(aimesh, scene, loadInfo.textures_, dir);
		meshs.push_back(mesh);
		primitive_models.push_back(aimesh->mPrimitiveTypes);
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, meshs,primitive_models,loadInfo, dir);
	}
}

#endif // ENGINE_ASSIMP


bool IO::NodeFile::assimpLoadModel(const char* path, LModelInfo& loadInfo)
{
#ifdef ENGINE_ASSIMP

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs
		| aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::stringstream err;
		err << "ERROR::ASSIMP:: " << importer.GetErrorString();
		Log::printMessage(err.str().c_str());
		return false;
	}
	std::string tp(path);
	std::string directory = tp.substr(0, tp.find_last_of('/'));

	std::vector<Mesh_SP > meshs;
	std::vector<unsigned int>		primitive_models;

	processNode(scene->mRootNode, scene, meshs,primitive_models, loadInfo, directory);

	for (int i = 0; i < primitive_models.size(); i++)
		loadInfo.mapMeshs[primitive_models[i]].push_back(meshs[i]);

	return true;
#endif // ENGINE_ASSIMP
	return false;
}
