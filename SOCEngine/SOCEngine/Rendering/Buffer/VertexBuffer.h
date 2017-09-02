#pragma once

#include "BaseBuffer.h"
#include "VertexShader.h"

namespace Rendering
{
	namespace Buffer
	{
		class VertexBuffer
		{
		public:
			using Semantics = std::vector<Shader::VertexShader::SemanticInfo>;
			struct Desc
			{
				Desc() = default;
				Desc(uint _stride, uint _vertexCount)
					: stride(_stride), vertexCount(_vertexCount) { }

				uint			stride		= 0;
				uint			vertexCount	= 0;
			};

			VertexBuffer() = default;

			void Initialize(Device::DirectX& dx, const Desc& desc, const void* sysMem, bool isDynamic, const std::vector<Shader::VertexShader::SemanticInfo>& semanticInfos);
			void IASetBuffer(Device::DirectX& dx);
			void UpdateVertexData(Device::DirectX& dx, const void* data, uint size);

			GET_CONST_ACCESSOR(VertexCount, uint, _desc.vertexCount);

			GET_CONST_ACCESSOR(Semantics, const Semantics&, _semantics);

		private:
			BaseBuffer	_baseBuffer;
			Desc		_desc;

			Semantics	_semantics; //attributes
		};
	}
}
