#pragma once

#include "Common.h"
#include "DefaultRenderTypes.h"
#include "Shaders.h"
#include "ShaderManager.h"
#include <string>
#include <unordered_map>

namespace Rendering
{
	class RenderingSystem;
	namespace Manager
	{
		class DefaultShaders
		{
		public:
			struct Shaders
			{
				Shader::VertexShader	vs;
				Shader::PixelShader		ps;
				Shader::GeometryShader	gs;

				Shaders() = default;
				Shaders(const Shader::ShaderGroup& shaderGroup)					 
				{
					if(shaderGroup.vs)	vs = *shaderGroup.vs;
					if(shaderGroup.gs)	gs = *shaderGroup.gs;
					if(shaderGroup.ps)	ps = *shaderGroup.ps;
				}					
			};

			friend class RenderingSystem;

		public:
			bool			Has(uint32 key) const;
			const Shaders&	Find(uint32 key) const;
			const Shaders&	Find(uint32 bufferFlag, DefaultRenderType renderType) const;

			static uint32 MakeKey(uint32 bufferFlag, DefaultRenderType renderType);
			std::string MakeDefaultSahderFileName(DefaultRenderType renderType, uint32 bufferFlag) const;

		private:
			void Initialize(Device::DirectX& dx, ShaderManager& shaderMgr);
			const Shaders& Add(uint32 key, const Shader::ShaderGroup& shaders);
			void Destroy();

			const Shaders& LoadDefaultSahder(	Device::DirectX& dx, ShaderManager& shaderMgr,
												DefaultRenderType renderType, uint32 bufferFlag, 
												const std::vector<Shader::ShaderMacro>* macros);
			void MakeDefaultShaderMainFuncNames(std::string& outVSMain, std::string& outGSMain, std::string& outPSMain, DefaultRenderType renderType);

		private:
			std::unordered_map<uint32, Shaders> _shaders; 
		};
	}
}