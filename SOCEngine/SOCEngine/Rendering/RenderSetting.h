#pragma once

#include <string>
#include "Rect.h"
#include "Common.h"

#include "GIInitParam.h"

struct RenderSetting
{
	std::string			mainCamName			= "";
	Rect<uint>			renderRect			= Rect<uint>(0, 0, 0, 0);
	uint				shadowMapResolution	= 1024;
	GIInitParam			giParam;
	bool				useBloom			= false;

	RenderSetting() = default;
	RenderSetting(const std::string& _mainCamName, const Rect<uint>& _renderRect, uint _shadowMapResolution, const GIInitParam& _giParam, bool _useBloom)
		: mainCamName(_mainCamName), renderRect(_renderRect), shadowMapResolution(_shadowMapResolution), giParam(_giParam), useBloom(_useBloom)
	{
	}
};