/* The Halfling Project - A Graphics Engine and Projects
 *
 * The Halfling Project is the legal property of Adrian Astley
 * Copyright Adrian Astley 2013
 */

#ifndef COMMON_MATERIALS_H
#define COMMON_MATERIALS_H

#include "DirectXMath.h"


namespace Common {

struct BlinnPhongMaterial {
	DirectX::XMFLOAT4 Ambient; // w = SpecularIntensity
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular; // w = SpecPower
};

} // End of namespace Common

#endif