#pragma once

#include "BaseBuffer.h"
#include "Utility.hpp"

namespace Rendering
{
	namespace Buffer
	{
		class VertexBuffer;
		class IndexBuffer final
		{		
		public:
			IndexBuffer() = default;

			void Initialize(Device::DirectX& dx, const std::vector<uint32>& indices, BaseBuffer::Key vbKey, bool isDynamic = false);
			void Destroy();
			void IASetBuffer(Device::DirectX& dx) const;

			GET_CONST_ACCESSOR(IndexCount,	uint32,				_indexCount);
			GET_CONST_ACCESSOR(VBKey,		BaseBuffer::Key,	_vbKey);

		private:
			uint32					_indexCount		= 0;
			BaseBuffer::Key			_vbKey;
			BaseBuffer				_baseBuffer;
		};
	}
}
