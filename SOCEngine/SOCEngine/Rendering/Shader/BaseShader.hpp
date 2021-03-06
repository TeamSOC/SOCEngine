#pragma once

#include <string>
#include "DirectX.h"
#include "Texture2D.h"
#include "ShaderResourceBuffer.h"
#include <vector>
#include "BindIndexInfo.h"
#include "ConstBuffer.h"

#define BASIC_SHADER_NAME "Basic"
#define BASIC_NORMAL_MAPPING_SHADER_NAME "BasicNormalMapping"

#define BASIC_VS_MAIN_FUNC_NAME "VS"
#define BASIC_PS_MAIN_FUNC_NAME "PS"

namespace Rendering
{
	namespace Shader
	{
		class BaseShader final
		{
		public:
			BaseShader() = default;
			BaseShader(const DXSharedResource<ID3DBlob>& blob, const std::string& key) : _blob(blob), _key(key) {}

			void Destroy() { _key = ""; _blob.Destroy(); }

			GET_CONST_ACCESSOR(Blob,		DXSharedResource<ID3DBlob>,		_blob);
			GET_CONST_ACCESSOR(Key,			const std::string&,				_key);
			GET_CONST_ACCESSOR(IsCanUse,	bool,							_blob.IsCanUse());

		private:
			std::string					_key = "";
			DXSharedResource<ID3DBlob>	_blob;
		};
	}
}
