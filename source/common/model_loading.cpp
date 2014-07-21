/* The Halfling Project - A Graphics Engine and Projects
 *
 * The Halfling Project is the legal property of Adrian Astley
 * Copyright Adrian Astley 2013 - 2014
 */

#include "common/model_loading.h"

#include "common/geometry_generator.h"
#include "common/model.h"
#include "common/model_manager.h"
#include "common/texture_manager.h"


namespace Common {
	
TextureSampler ParseSamplerTypeFromString(std::string &inputString, TextureSampler defaultType) {
	if (_stricmp(inputString.c_str(), "linear_clamp") == 0) {
		return LINEAR_CLAMP;
	} else if (_stricmp(inputString.c_str(), "linear_border") == 0) {
		return LINEAR_BORDER;
	} else if (_stricmp(inputString.c_str(), "linear_wrap") == 0) {
		return LINEAR_WRAP;
	} else if (_stricmp(inputString.c_str(), "point_clamp") == 0) {
		return POINT_CLAMP;
	} else if (_stricmp(inputString.c_str(), "point_wrap") == 0) {
		return POINT_WRAP;
	} else if (_stricmp(inputString.c_str(), "anisotropic_wrap") == 0) {
		return ANISOTROPIC_WRAP;
	} else {
		return defaultType;
	}
}


Model *FileModelToLoad::CreateModel(ID3D11Device *device, Common::TextureManager *textureManager, Common::ModelManager *modelManager) {
	return m_modelManager->GetModel(m_device, m_textureManager, m_filePath.c_str());
}

struct Vertex {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 texCoord;
	DirectX::XMFLOAT3 tangent;
};

Model *PlaneModelToLoad::CreateModel(ID3D11Device *device, Common::TextureManager *textureManager, Common::ModelManager *modelManager) {
	Common::GeometryGenerator::MeshData meshData;
	Common::ModelSubset *subset = new Common::ModelSubset[1];

	Common::GeometryGenerator::CreateGrid(m_width, m_depth, m_x_subdivisions, m_z_subdivisions, &meshData, m_x_textureTiling, m_z_textureTiling);
	subset->AABB_min = DirectX::XMFLOAT3(-m_width * 0.5f, 0.0f, -m_depth * 0.5f);
	subset->AABB_max = DirectX::XMFLOAT3(m_width * 0.5f, 0.0f, m_depth * 0.5f);

	subset->IndexStart = 0u;
	subset->IndexCount = static_cast<uint>(meshData.Indices.size());
	subset->VertexStart = 0u;
	subset->VertexCount = static_cast<uint>(meshData.Vertices.size());

	Vertex *vertices = new Vertex[meshData.Vertices.size()];
	for (uint i = 0; i < meshData.Vertices.size(); ++i) {
		vertices[i].pos = meshData.Vertices[i].Position;
		vertices[i].normal = meshData.Vertices[i].Normal;
		vertices[i].texCoord = meshData.Vertices[i].TexCoord;
		vertices[i].tangent = meshData.Vertices[i].Tangent;
	}

	Common::Model *newModel = modelManager->CreateUnnamedModel();
	newModel->CreateVertexBuffer(device, vertices, sizeof(Vertex), static_cast<uint>(meshData.Vertices.size()));
	newModel->CreateIndexBuffer(device, &meshData.Indices[0], static_cast<uint>(meshData.Indices.size()), DisposeAfterUse::NO);
	newModel->CreateSubsets(subset, 1);

	return newModel;
}

Model *BoxModelToLoad::CreateModel(ID3D11Device *device, Common::TextureManager *textureManager, Common::ModelManager *modelManager) {
	Common::GeometryGenerator::MeshData meshData;
	Common::ModelSubset *subset = new Common::ModelSubset[1];

	Common::GeometryGenerator::CreateBox(m_width, m_height, m_depth, &meshData);
	subset->AABB_min = DirectX::XMFLOAT3(-m_width * 0.5f, -m_height * 0.5f, -m_depth * 0.5f);
	subset->AABB_max = DirectX::XMFLOAT3(m_width * 0.5f, m_height * 0.5f, m_depth * 0.5f);

	subset->IndexStart = 0u;
	subset->IndexCount = static_cast<uint>(meshData.Indices.size());
	subset->VertexStart = 0u;
	subset->VertexCount = static_cast<uint>(meshData.Vertices.size());

	Vertex *vertices = new Vertex[meshData.Vertices.size()];
	for (uint i = 0; i < meshData.Vertices.size(); ++i) {
		vertices[i].pos = meshData.Vertices[i].Position;
		vertices[i].normal = meshData.Vertices[i].Normal;
		vertices[i].texCoord = meshData.Vertices[i].TexCoord;
		vertices[i].tangent = meshData.Vertices[i].Tangent;
	}

	Common::Model *newModel = modelManager->CreateUnnamedModel();
	newModel->CreateVertexBuffer(device, vertices, sizeof(Vertex), static_cast<uint>(meshData.Vertices.size()));
	newModel->CreateIndexBuffer(device, &meshData.Indices[0], static_cast<uint>(meshData.Indices.size()), DisposeAfterUse::NO);
	newModel->CreateSubsets(subset, 1);

	return newModel;
}

Model *SphereModelToLoad::CreateModel(ID3D11Device *device, Common::TextureManager *textureManager, Common::ModelManager *modelManager) {
	Common::GeometryGenerator::MeshData meshData;
	Common::ModelSubset *subset = new Common::ModelSubset[1];

	Common::GeometryGenerator::CreateSphere(m_radius, m_sliceCount, m_stackCount, &meshData);
	subset->AABB_min = DirectX::XMFLOAT3(-m_radius, -m_radius, -m_radius);
	subset->AABB_max = DirectX::XMFLOAT3(m_radius, m_radius, m_radius);

	subset->IndexStart = 0u;
	subset->IndexCount = static_cast<uint>(meshData.Indices.size());
	subset->VertexStart = 0u;
	subset->VertexCount = static_cast<uint>(meshData.Vertices.size());

	Vertex *vertices = new Vertex[meshData.Vertices.size()];
	for (uint i = 0; i < meshData.Vertices.size(); ++i) {
		vertices[i].pos = meshData.Vertices[i].Position;
		vertices[i].normal = meshData.Vertices[i].Normal;
		vertices[i].texCoord = meshData.Vertices[i].TexCoord;
		vertices[i].tangent = meshData.Vertices[i].Tangent;
	}

	Common::Model *newModel = modelManager->CreateUnnamedModel();
	newModel->CreateVertexBuffer(device, vertices, sizeof(Vertex), static_cast<uint>(meshData.Vertices.size()));
	newModel->CreateIndexBuffer(device, &meshData.Indices[0], static_cast<uint>(meshData.Indices.size()), DisposeAfterUse::NO);
	newModel->CreateSubsets(subset, 1);

	return newModel;
}

} // End of namespace Common