#pragma once

#include "ShaderForm.h"

namespace Rendering
{
	namespace Shader
	{
		class PixelShader : public ShaderForm
		{
		private:
			ID3D11PixelShader* _shader;

		public:
			PixelShader(ID3DBlob* blob);
			~PixelShader(void);

		public:
			bool Create(ID3D11Device* device);
			
			void BindShaderToContext(ID3D11DeviceContext* context);
			void BindResourcesToContext(ID3D11DeviceContext* context, 
				const std::vector<InputConstBuffer>*			constBuffers, 
				const std::vector<InputTexture>*				textures,
				const std::vector<InputShaderResourceBuffer>*	srBuffers);

			void Clear(ID3D11DeviceContext* context,
				const std::vector<InputConstBuffer>*			constBuffers, 
				const std::vector<InputTexture>*				textures,
				const std::vector<InputShaderResourceBuffer>*	srBuffers);
		};
	}
}
