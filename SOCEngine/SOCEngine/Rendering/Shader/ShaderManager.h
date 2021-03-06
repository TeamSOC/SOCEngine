#pragma once

#include <string>
#include <fstream>
#include "ShaderPool.hpp"
#include "PixelShader.h"
#include "GeometryShader.h"
#include "ShaderMacro.hpp"
#include "VectorIndexer.hpp"
#include "DirectX.h"
#include "ShaderMacro.hpp"
#include <tuple>
#include "ShaderCompiler.h"

namespace Rendering
{
	namespace Manager
	{
		using PixelShaderManager	= ShaderPool<Shader::PixelShader>;
		using GeometryShaderManager	= ShaderPool<Shader::GeometryShader>;

		class ShaderManager final
		{
		public:
			ShaderManager() = default;
			DISALLOW_ASSIGN_COPY(ShaderManager);

			void Destroy();

			template<class ShaderType>
			inline ShaderPool<ShaderType>& GetPool()
			{
				return std::get<ShaderPool<ShaderType>>(_pools);
			}

			GET_ALL_ACCESSOR_REF(Compiler, _compiler);

		private:
			std::tuple<
				ShaderPool<Shader::VertexShader>,
				ShaderPool<Shader::GeometryShader>,
				ShaderPool<Shader::PixelShader>,
				ShaderPool<Shader::ComputeShader>> _pools;
			Shader::ShaderCompiler				_compiler;
		};
	}
}