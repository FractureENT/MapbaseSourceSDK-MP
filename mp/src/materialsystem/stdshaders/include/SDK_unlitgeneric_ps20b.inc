#pragma once
#include "shaderlib/cshader.h"
class SDK_unlitgeneric_ps20b_Static_Index
{
public:
	SDK_unlitgeneric_ps20b_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderStaticTest_SDK_unlitgeneric_ps20b 1


class SDK_unlitgeneric_ps20b_Dynamic_Index
{
public:
	SDK_unlitgeneric_ps20b_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_SDK_unlitgeneric_ps20b 1

