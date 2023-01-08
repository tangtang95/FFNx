/****************************************************************************/
//    Copyright (C) 2022 Cosmos                                             //
//                                                                          //
//    This file is part of FFNx                                             //
//                                                                          //
//    FFNx is free software: you can redistribute it and/or modify          //
//    it under the terms of the GNU General Public License as published by  //
//    the Free Software Foundation, either version 3 of the License         //
//                                                                          //
//    FFNx is distributed in the hope that it will be useful,               //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
//    GNU General Public License for more details.                          //
/****************************************************************************/

$input a_position, a_color0, a_texcoord0, a_normal
$output v_color0, v_texcoord0, v_position0, v_shadow0, v_normal0

#include <bgfx/bgfx_shader.sh>

uniform mat4 d3dViewport;
uniform mat4 d3dProjection;
uniform mat4 viewMatrix;
uniform mat4 invViewMatrix;
uniform mat4 worldView;
uniform mat4 normalMatrix;
uniform mat4 lightViewProjTexMatrix;

uniform vec4 VSFlags;
uniform vec4 WMFlags;
uniform vec4 lightingDebugData;

#define isTLVertex VSFlags.x > 0.0
#define blendMode VSFlags.y
#define isFBTexture VSFlags.z > 0.0
#define isNotTexture VSFlags.w == 0.0

#define isApplySphericalWorld WMFlags.x > 0.0

#define isHide2dEnabled lightingDebugData.x > 0.0

void main()
{
	vec4 pos = a_position;
    vec4 color = a_color0;
    vec2 coords = a_texcoord0;

    color.rgba = color.bgra;

    if (isTLVertex)
    {
        pos.w = 1.0 / pos.w;
        pos.xyz *= pos.w;
        pos = mul(u_proj, pos);

        if (isHide2dEnabled) pos = vec4_splat(0.0);
    }
    else
    {
        v_position0 = mul(worldView, vec4(pos.xyz, 1.0));

        if (isApplySphericalWorld)
        {
            float rp = -300000;

            vec2 planedir = normalize(vec2(v_position0.x, v_position0.z));
            vec2 plane = vec2(v_position0.y + rp, sqrt((v_position0.x) * (v_position0.x) + (v_position0.z) * (v_position0.z)));
            vec2 circle = plane.x * vec2(cos(plane.y / rp), sin(plane.y / rp)) - vec2(rp, 0);
            pos = vec4(circle.y * planedir.x, circle.x, circle.y * planedir.y, 1.0);
        }
        else
        {
            pos = v_position0;
        }

        v_shadow0 = mul(lightViewProjTexMatrix, v_position0);
        v_normal0 = mul(normalMatrix, vec4(a_normal, 0.0)).xyz;
        pos = mul(mul(d3dViewport, d3dProjection), pos);

        if (color.a > 0.5) color.a = 0.5;
    }

    if (blendMode == 4.0) color.a = 1.0;
    else if (blendMode == 3.0) color.a = 0.25;

#if BGFX_SHADER_LANGUAGE_HLSL
#else
    #if BGFX_SHADER_LANGUAGE_SPIRV
    #else
        if (isFBTexture) coords.y = 1.0 - coords.y;
    #endif
#endif

    gl_Position = pos;
    v_color0 = color;
    v_texcoord0 = coords;
}
