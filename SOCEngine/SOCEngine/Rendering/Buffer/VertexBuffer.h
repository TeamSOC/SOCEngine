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
			using Key = BaseBuffer::Key;
			using Semantics = std::vector<Shader::VertexShader::SemanticInfo>;
			struct Desc
			{
				Desc() = default;
				Desc(uint32 _stride, uint32 _vertexCount)
					: stride(_stride), vertexCount(_vertexCount) { }

				uint32			stride		= 0;
				uint32			vertexCount	= 0;
			};

			VertexBuffer() = default;

			void Initialize(Device::DirectX& dx, const Desc& desc, const void* sysMem, bool isDynamic, const std::vector<Shader::VertexShader::SemanticInfo>& semanticInfos);
			void Destroy();

			void IASetBuffer(Device::DirectX& dx, uint32 offset = 0) const;
			void UpdateVertexData(Device::DirectX& dx, const void* data, uint32 size);

			GET_CONST_ACCESSOR(VertexCount,	uint32,			_desc.vertexCount);
			GET_CONST_ACCESSOR(Semantics,	const auto&,	_semantics);

		private:
			BaseBuffer	_baseBuffer;
			Desc		_desc;

			Semantics	_semantics; //attributes
		};
	}
}
