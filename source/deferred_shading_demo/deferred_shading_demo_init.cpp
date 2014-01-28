/* The Halfling Project - A Graphics Engine and Projects
 *
 * The Halfling Project is the legal property of Adrian Astley
 * Copyright Adrian Astley 2013
 */

#include "deferred_shading_demo/deferred_shading_demo.h"

#include "deferred_shading_demo/shader_constants.h"

#include "common/geometry_generator.h"
#include "common/math.h"


namespace DeferredShadingDemo {

bool DeferredShadingDemo::Initialize(LPCTSTR mainWndCaption, uint32 screenWidth, uint32 screenHeight, bool fullscreen) {
	if (!Halfling::HalflingEngine::Initialize(mainWndCaption, screenWidth, screenHeight, fullscreen))
		return false;

	InitTweakBar();

	BuildGeometryBuffers();
	CreateLights();

	LoadShaders();
	CreateShaderBuffers();

	// Set the view matrices to identity
	DirectX::XMMATRIX identity = DirectX::XMMatrixIdentity();
	m_worldViewProj.world = identity;
	m_worldViewProj.view = identity;
	m_worldViewProj.view = identity;

	m_spriteRenderer.Initialize(m_device);
	m_timesNewRoman12Font.Initialize(L"Times New Roman", 12, Common::SpriteFont::Regular, true, m_device);

	m_blendStates.Initialize(m_device);
	m_depthStencilStates.Initialize(m_device);
	m_rasterizerStates.Initialize(m_device);
	m_samplerStates.Initialize(m_device);

	return true;
}

void DeferredShadingDemo::InitTweakBar() {
	int success = TwInit(TW_DIRECT3D11, m_device);

	m_settingsBar = TwNewBar("settings");
	TwDefine(" settings label='Settings' size='200 300' movable=true resizable=false fontresizable=false contained=true iconified=true ");

	TwAddVarRW(m_settingsBar, "V-Sync", TwType::TW_TYPE_BOOLCPP, &m_vsync, "");
	TwAddVarCB(m_settingsBar, "Wireframe", TwType::TW_TYPE_BOOLCPP, DeferredShadingDemo::SetWireframeRSCallback, DeferredShadingDemo::GetWireframeTSCallback, this, "");
}

void DeferredShadingDemo::BuildGeometryBuffers() {
	m_models.push_back(Common::Model<Vertex>());
	Common::Model<Vertex> *model = &m_models.back();

	Common::GeometryGenerator::MeshData meshData;
	Common::GeometryGenerator::CreateGrid(160.0f, 160.0f, 50, 50, &meshData, 3.0f, 3.0f);

	uint vertexCount = meshData.Vertices.size();
	uint indexCount = meshData.Indices.size();

	Vertex *vertices = new Vertex[vertexCount];
	for (uint i = 0; i < vertexCount; ++i) {
		vertices[i].pos = meshData.Vertices[i].Position;
		vertices[i].pos.y = GetHillHeight(vertices[i].pos.x, vertices[i].pos.z);
		vertices[i].normal = GetHillNormal(vertices[i].pos.x, vertices[i].pos.z);
		vertices[i].texCoord = meshData.Vertices[i].TexCoord;
	}
	model->SetVertices(m_device, vertices, vertexCount);

	uint *indices = new uint[indexCount];
	for (uint i = 0; i < indexCount; ++i) {
		indices[i] = meshData.Indices[i];
	}
	model->SetIndices(m_device, indices, indexCount);

	// Create subsets
	Common::ModelSubset *subsets = new Common::ModelSubset[1] {
		{0, vertexCount, 0, indexCount / 3, {DirectX::XMFLOAT4(0.48f, 0.77f, 0.46f, 0.0f),
		                                     DirectX::XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f),
		                                     DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f)},
			m_textureManager.GetSRVFromDDSFile(m_device, "grass.dds", D3D11_USAGE_IMMUTABLE)
		}
	};
	model->SetSubsets(subsets, 1);

	meshData.Indices.clear();
	meshData.Vertices.clear();

	Common::GeometryGenerator::CreateFullscreenQuad(meshData);

	vertexCount = meshData.Vertices.size();
	indexCount = meshData.Indices.size();

	FullScreenTriangleVertex *fullscreenQuadVertices = new FullScreenTriangleVertex[vertexCount];
	for (uint i = 0; i < vertexCount; ++i) {
		fullscreenQuadVertices[i].pos = meshData.Vertices[i].Position;
	}
	m_fullScreenQuad.SetVertices(m_device, fullscreenQuadVertices, vertexCount);

	uint *fullscreenQuadIndices = new uint[indexCount];
	for (uint i = 0; i < indexCount; ++i) {
		fullscreenQuadIndices[i] = meshData.Indices[i];
	}
	m_fullScreenQuad.SetIndices(m_device, fullscreenQuadIndices, indexCount);

	Common::ModelSubset *fullscreenQuadSubsets = new Common::ModelSubset[1] {
		{0, vertexCount, 0, indexCount / 3, {{0,0,0,0}}, nullptr}
	};
	m_fullScreenQuad.SetSubsets(fullscreenQuadSubsets, 1);
}

float DeferredShadingDemo::GetHillHeight(float x, float z) const {
	return 0.3f * ((z * sinf(0.1f * x)) + (x * cosf(0.1f * z)));
}

DirectX::XMFLOAT3 DeferredShadingDemo::GetHillNormal(float x, float z) const {
	DirectX::XMFLOAT3 normal(
		-0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z),
		1.0f,
		0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));

	DirectX::XMVECTOR unitNormal = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&normal));
	DirectX::XMStoreFloat3(&normal, unitNormal);

	return normal;
}

void DeferredShadingDemo::CreateLights() {
	m_directionalLight.Ambient = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_directionalLight.Diffuse = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_directionalLight.Specular = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_directionalLight.Direction = DirectX::XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	for (uint i = 0; i < 100; ++i) {
		Common::PointLight pointLight;
		pointLight.Diffuse = pointLight.Specular = DirectX::XMFLOAT4(Common::RandF(), Common::RandF(), Common::RandF(), 1.0f);
		pointLight.AttenuationDistanceUNorm = 0.75;
		pointLight.Range = 25.0f;
		pointLight.Position = DirectX::XMFLOAT3(Common::RandF(-80.0f, 80.0f), Common::RandF(-40.0f, 40.0f), Common::RandF(-80.0f, 80.0f));

		m_pointLights.push_back(pointLight);
	}

	m_pointLightBufferNeedsRebuild = true;
}

void DeferredShadingDemo::LoadShaders() {
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	D3D11_INPUT_ELEMENT_DESC fullscreenTriangleVertexDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HR(Common::LoadVertexShader("gbuffer_vertex_shader.cso", m_device, &m_gbufferVertexShader, &m_gBufferInputLayout, vertexDesc, 3));
	HR(Common::LoadPixelShader("gbuffer_pixel_shader.cso", m_device, &m_gbufferPixelShader));
	HR(Common::LoadVertexShader("fullscreen_triangle_vertex_shader.cso", m_device, &m_fullscreenTriangleVertexShader, &m_fullscreenTriangleInputLayout, fullscreenTriangleVertexDesc, 1));
	HR(Common::LoadPixelShader("no_cull_final_gather_pixel_shader.cso", m_device, &m_noCullFinalGatherPixelShader));
}

void DeferredShadingDemo::CreateShaderBuffers() {

	D3D11_BUFFER_DESC vertexShaderObjectBufferDesc;
	vertexShaderObjectBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexShaderObjectBufferDesc.ByteWidth = Common::CBSize(sizeof(GBufferVertexShaderObjectConstants));
	vertexShaderObjectBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vertexShaderObjectBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexShaderObjectBufferDesc.MiscFlags = 0;
	vertexShaderObjectBufferDesc.StructureByteStride = 0;

	m_device->CreateBuffer(&vertexShaderObjectBufferDesc, NULL, &m_gBufferVertexShaderObjectConstantsBuffer);

	D3D11_BUFFER_DESC pixelShaderFrameBufferDesc;
	pixelShaderFrameBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelShaderFrameBufferDesc.ByteWidth = Common::CBSize(sizeof(GBufferPixelShaderObjectConstants));
	pixelShaderFrameBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelShaderFrameBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelShaderFrameBufferDesc.MiscFlags = 0;
	pixelShaderFrameBufferDesc.StructureByteStride = 0;

	m_device->CreateBuffer(&pixelShaderFrameBufferDesc, NULL, &m_gBufferPixelShaderObjectConstantsBuffer);

	D3D11_BUFFER_DESC noCullFinalGatherPixelShaderFrameBufferDesc;
	noCullFinalGatherPixelShaderFrameBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	noCullFinalGatherPixelShaderFrameBufferDesc.ByteWidth = Common::CBSize(sizeof(NoCullFinalGatherPixelShaderFrameConstants));
	noCullFinalGatherPixelShaderFrameBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	noCullFinalGatherPixelShaderFrameBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	noCullFinalGatherPixelShaderFrameBufferDesc.MiscFlags = 0;
	noCullFinalGatherPixelShaderFrameBufferDesc.StructureByteStride = 0;

	m_device->CreateBuffer(&noCullFinalGatherPixelShaderFrameBufferDesc, NULL, &m_noCullFinalGatherPixelShaderConstantsBuffer);

	if (m_pointLights.size() > 0)
		m_pointLightBuffer = new Common::StructuredBuffer<Common::PointLight>(m_device, m_pointLights.size(), D3D11_BIND_SHADER_RESOURCE, true);
	if (m_spotLights.size() > 0)
		m_spotLightBuffer = new Common::StructuredBuffer<Common::SpotLight>(m_device, m_spotLights.size(), D3D11_BIND_SHADER_RESOURCE, true);

	m_frameMaterialListBuffer = new Common::StructuredBuffer<Common::BlinnPhongMaterial>(m_device, kMaxMaterialsPerFrame, D3D11_BIND_SHADER_RESOURCE, true);

	D3D11_SAMPLER_DESC diffuseSamplerDesc;
	memset(&diffuseSamplerDesc, 0, sizeof(D3D11_SAMPLER_DESC));
	diffuseSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	diffuseSamplerDesc.MaxAnisotropy = 4;
	diffuseSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	diffuseSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	diffuseSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	HR(m_device->CreateSamplerState(&diffuseSamplerDesc, &m_diffuseSampleState));
}

} // End of namespace DeferredShadingDemo