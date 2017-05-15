#pragma once

#include "BaseShader.hpp"
#include "ConstBuffer.h"
#include "BindIndexInfo.h"

namespace Rendering
{
	namespace Shader
	{
		class VertexShader final
		{
		public:
			struct SemanticInfo
			{
				std::string	name = "";
				uint		semanticIndex = -1;
				uint		size = 0;
			};

			VertexShader(const DXResource<ID3DBlob>& blob, const std::string& key);
			GET_CONST_ACCESSOR(SemanticInfos, const std::vector<SemanticInfo>&, _semanticInfo);
			GET_CONST_ACCESSOR(Key, const std::string&, _baseShader.GetKey());

			void Initialize(Device::DirectX& dx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations);

			void BindShaderToContext(Device::DirectX& dx);
			void BindInputLayoutToContext(Device::DirectX& dx);
			void UnBindShaderToContext(Device::DirectX& dx);
			void UnBindInputLayoutToContext(Device::DirectX& dx);

			static void BindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind, IN View::ShaderResourceView& view);
			static void BindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind, IN DXResource<ID3D11SamplerState>& samplerState);
			static void BindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind, IN Buffer::ConstBuffer& cb);

			static void UnBindShaderResourceView(Device::DirectX& dx, TextureBindIndex bind);
			static void UnBindSamplerState(Device::DirectX& dx, SamplerStateBindIndex bind);
			static void UnBindConstBuffer(Device::DirectX& dx, ConstBufferBindIndex bind);

			static constexpr const char* GetCompileCode() { return "vs"; }

		private:
			BaseShader			_baseShader;
			DXResource<ID3D11VertexShader>	_shader;
			DXResource<ID3D11InputLayout>	_layout;

			std::vector<SemanticInfo> _semanticInfo;
		};
	}
}
