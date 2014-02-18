/* The Halfling Project - A Graphics Engine and Projects
 *
 * The Halfling Project is the legal property of Adrian Astley
 * Copyright Adrian Astley 2013
 */

#ifndef OBJ_LOADER_DEMO_SHADING_CONSTANTS_H
#define OBJ_LOADER_DEMO_SHADING_CONSTANTS_H

#include "common/materials.h"
#include "common/lights.h"

#include "DirectXMath.h"


namespace ObjLoaderDemo {

// Forward Shading
struct ForwardPixelShaderFrameConstants {
	Common::DirectionalLight gDirectionalLight;
	DirectX::XMFLOAT3 gEyePosition;
	float pad;

	uint gNumPointLightsToDraw;
	uint gNumSpotLightsToDraw;
	uint pad2[2];
};

struct ForwardPixelShaderObjectConstants {
	Common::BlinnPhongMaterial Material;
};


// Gbuffer pass
struct GBufferVertexShaderObjectConstants {
	DirectX::XMMATRIX WorldViewProj;
	DirectX::XMMATRIX World;
};

struct GBufferPixelShaderObjectConstants {
	uint MaterialIndex;
};

// No cull final gather pass
struct NoCullFinalGatherPixelShaderFrameConstants {
	DirectX::XMMATRIX gProjection;
	DirectX::XMMATRIX gInvViewProjection;

	Common::DirectionalLight gDirectionalLight;
	DirectX::XMFLOAT3 gEyePosition;
	float pad;

	uint gNumPointLightsToDraw;
	uint gNumSpotLightsToDraw;
	uint pad2[2];
};

// Debug pass
struct TransformedFullScreenTriangleVertexShaderConstants {
	DirectX::XMFLOAT2 gClipTranslation;
	float gClipScale;
};

struct RenderGBuffersPixelShaderConstants {
	DirectX::XMMATRIX gProj;
	DirectX::XMMATRIX gInvViewProjection;
	uint gGBufferIndex;
};

} // End of namespace ObjLoaderDemo

#endif