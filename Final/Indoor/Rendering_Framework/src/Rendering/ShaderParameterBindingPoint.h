#pragma once

#include <glad/glad.h>

namespace INANOA
{
	namespace SHADER_PARAMETER_BINDING
	{
		const GLuint VERTEX_LOCATION = 0;

		const GLint MODEL_MAT_LOCATION = 0;
		const GLint VIEW_MAT_LOCATION = 1;
		const GLint PROJ_MAT_LOCATION = 2;

		const GLint IS_TEXTURE_NORMALIZED = 9;
		const GLint LIGHT_SPACE_MATRIX = 10;
		const GLint LIGHT_POS = 11;
		const GLint SHADOW_SBPV_MATRIX = 12;
		const GLint SHADOW_MAP = 13;
		const GLint GBUFFER_TEXTURE = 14;
		const GLint DEPTHMVP_TEXTURE = 15;
		const GLint SHADOW_MAP_ENABLE = 16;
		const GLint POINT_LIGHT_ENABLE = 17;
		const GLint DIR_LIGHT_POS = 18;
		const GLint PNT_LIGHT_POS = 19;

		const GLint USEBP_LIGHT_LOCATION = 20;
		const GLint USE_BLOOM_LOCATION = 21;
		const GLint HORIZONTAL_LOCATION = 22;
		const GLint Ns_LOCATION = 23;
		const GLint VIEW_MAT_POS = 24;
		const GLint VOL_LIGHT_ENABLE = 25;
		const GLint PROJ_MAT_POS = 26;

		const GLint IS_SPHERE_LOCATION = 28;
		const GLint NORMAL_MAP_USED_LOCATION = 29;
		const GLint TEX_USED_LOCATION = 30;
		const GLint Kd_LOCATION = 31;
		const GLint Ka_LOCATION = 32;
		const GLint Ks_LOCATION = 33;
		const GLint NORMAL_MAP_TEXTURE = 34;

		const GLint POSITION_TEXTURE = 30;
		const GLint NORMAL_TEXTURE = 31;
		const GLint AMBIENT_TEXTURE = 32;
		const GLint DIFFUSE_TEXTURE = 33;
		const GLint SPECULAR_TEXTURE = 34;

		const GLint DEPTH_TEXTURE = 35;
		const GLint NOISE_TEXTURE = 36;
		const GLint SSAO_ENABLE = 37;
		const GLint SSAO_ONLY_ENABLE = 38;
		const GLint FRAME_SIZE = 39;
		const GLint OBSTACLE_TEXTURE = 40;
		const GLint LIGHT_POS_ON_SCREEN = 41;
		const GLint NPR_ENABLE = 42;

		const GLint FAR_PLANE = 43;
		const GLint POINT_SHADOW_MAP = 44;
		const GLint POINT_SHADOW_MAP_ENABLE = 45;
		const GLint WORLD_POSITION_TEXTURE = 46;
		const GLint SHADOW_TRANSFORMS = 47;
	}
}
