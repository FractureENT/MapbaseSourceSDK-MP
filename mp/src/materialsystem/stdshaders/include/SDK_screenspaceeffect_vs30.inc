// ALL SKIP STATEMENTS THAT AFFECT THIS SHADER!!!
// defined $LIGHTING_PREVIEW && defined $FASTPATH && $LIGHTING_PREVIEW && $FASTPATH

#pragma once
#include "shaderlib/cshader.h"
class SDK_screenspaceeffect_vs30_Static_Index
{
	unsigned int m_nX360APPCHOOSER : 2;
public:
	void SetX360APPCHOOSER( int i )
	{
		Assert( i >= 0 && i <= 1 );
		m_nX360APPCHOOSER = i;
	}

	SDK_screenspaceeffect_vs30_Static_Index( IShaderShadow* pShaderShadow, IMaterialVar** params )
	{
		m_nX360APPCHOOSER = 0;
	}

	int GetIndex() const
	{
		return ( 1 * m_nX360APPCHOOSER ) + 0;
	}
};

#define shaderStaticTest_SDK_screenspaceeffect_vs30 1


class SDK_screenspaceeffect_vs30_Dynamic_Index
{
public:
	SDK_screenspaceeffect_vs30_Dynamic_Index( IShaderDynamicAPI* pShaderAPI )
	{
	}

	int GetIndex() const
	{
		return 0;
	}
};

#define shaderDynamicTest_SDK_screenspaceeffect_vs30 1

