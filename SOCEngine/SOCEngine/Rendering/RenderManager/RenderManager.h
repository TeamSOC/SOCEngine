#pragma once

#include <vector>
#include "Mesh.h"
#include "Utility.h"
#include <functional>

#include "DeferredCamera.h"
#include "BackBufferMaker.h"

#include "GlobalSetting.h"

#include <sys/timeb.h>
#include <time.h>
#include "VectorMap.h"
#include <hash_map>

#include "ShaderMacro.h"

namespace Rendering
{
	namespace Manager
	{
		class RenderManager
		{
		public:
			enum class MeshType
			{
				Opaque,
				Transparent,
				AlphaTest
			};
			struct MeshList
			{
				uint updateCounter;

				//first value is just key.
				Structure::VectorMap<unsigned int, const Mesh::Mesh*> meshes;

				MeshList(){}
				~MeshList(){}
			};
			enum class DefaultVertexInputType : uint
			{
				UV, //Only UV
				N_UV, //Normal, UV
				TBN_UV //Tangent, Binormal, Normal, UV
			};

		private:
			MeshList	_transparentMeshes;
			MeshList	_opaqueMeshes;
			MeshList	_alphaTestMeshes;

			std::hash_map<DefaultVertexInputType, const Shader::ShaderGroup>	_gbufferShaders;
			std::hash_map<DefaultVertexInputType, const Shader::ShaderGroup>	_gbufferShaders_alphaTest;

			std::hash_map<DefaultVertexInputType, const Shader::ShaderGroup>	_transparentShaders;

		public:
			RenderManager();
			~RenderManager();

		public:
			bool TestInit();
			Shader::ShaderGroup LoadDefaultSahder(MeshType meshType, DefaultVertexInputType defaultVertexInputType,
				const std::string* customShaderFileName = nullptr, const std::vector<Rendering::Shader::ShaderMacro>* macros = nullptr);

			void UpdateRenderList(const Mesh::Mesh* mesh, MeshType type);
			const Mesh::Mesh* FindMeshFromRenderList(const Mesh::Mesh* mesh, MeshType type);

			bool FindGBufferShader(Shader::ShaderGroup& out, DefaultVertexInputType type, bool isAlphaTest);
			bool FindTransparencyShader(Shader::ShaderGroup& out, DefaultVertexInputType type);

		public:
			GET_ACCESSOR(TransparentMeshes, const MeshList&, _transparentMeshes);
			GET_ACCESSOR(OpaqueMeshes, const MeshList&, _opaqueMeshes);
			GET_ACCESSOR(AlphaTestMeshes, const MeshList&, _alphaTestMeshes);
		};
	}
}