//=================================================================================================
//
//	MJP's DX11 Sample Framework
//  http://mynameismjp.wordpress.com/
//
//  All code and content licensed under Microsoft Public License (Ms-PL)
//
//=================================================================================================

/**
 * Modified for use in The Halfling Project - A Graphics Engine and Projects
 * The Halfling Project is the legal property of Adrian Astley
 * Copyright Adrian Astley 2013
 */

#include "common/shaders/sprite_header.hlsli"

//=================================================================================================
// Vertex Shader, non-instanced
//=================================================================================================
VSOutput SpriteVS(in VSInput input) {
    return SpriteVSCommon(input.Position, input.TexCoord, Transform, Color, SourceRect);
}