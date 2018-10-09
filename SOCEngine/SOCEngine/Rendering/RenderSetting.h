#pragma once

#include <string>
#include "Rect.h"
#include "Common.h"

#include "GIInitParam.h"

struct RenderSetting
{
	std::string			mainCamName			= "";
	Rect<uint32>			renderRect			= Rect<uint32>(0, 0, 0, 0);
	uint32				shadowMapResolution	= 1024;
	GIInitParam			giParam;
	bool				useBloom			= false;

	RenderSetting() = default;
	RenderSetting(const std::string& _mainCamName, const Rect<uint32>& _renderRect, uint32 _shadowMapResolution, const GIInitParam& _giParam, bool _useBloom)
		: mainCamName(_mainCamName), renderRect(_renderRect), shadowMapResolution(_shadowMapResolution), giParam(_giParam), useBloom(_useBloom)
	{
	}
};