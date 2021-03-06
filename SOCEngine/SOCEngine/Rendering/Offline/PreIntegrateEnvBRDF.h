#pragma once

#include "DirectX.h"
#include "Texture2D.h"
#include "ShaderManager.h"

namespace Rendering
{
	namespace Precompute
	{
		class PreIntegrateEnvBRDF final
		{
		public:
			Texture::Texture2D& CreatePreBRDFMap(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);
			void Destroy();

			GET_CONST_ACCESSOR_REF(Texture2D, _texture);

		private:
			Texture::Texture2D		_texture;
		};
	}
}