#pragma once

#include "Vector2.h"
#include "Vector3.h"

#include "Mesh.h"
#include "Object.h"
#include "DefaultRenderTypes.h"

namespace Core
{
	class Engine;
}

namespace Rendering
{
	namespace Geometry
	{
		class BasicGeometryGenerator
		{
		public:
			static void CreateBox(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback,			const Math::Vector3& size, uint32 defautVertexInputTypeFlag);
			static void CreateSphere(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback,		float radius, uint32 sliceCount, uint32 stackCount, uint32 defautVertexInputTypeFlag);
			static void CreateCylinder(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback,	float botRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, uint32 defautVertexInputTypeFlag);
			static void CreatePlane(std::function<void(const Mesh::CreateFuncArguments&)> createMeshCallback,		float width, float height, uint32 widthVertexCount, uint32 heightVertexCount, uint32 defautVertexInputTypeFlag);

			static constexpr uint32 DefaultFlag = uint32(DefaultVertexInputTypeFlag::UV0) | uint32(DefaultVertexInputTypeFlag::NORMAL);
			static void CreateBox(Core::Object& targetObj,		Core::Engine& engine, const Math::Vector3& size, uint32 defautVertexInputTypeFlag = DefaultFlag);
			static void CreateSphere(Core::Object& targetObj,	Core::Engine& engine, float radius, uint32 sliceCount, uint32 stackCount, uint32 defautVertexInputTypeFlag = DefaultFlag);
			static void CreateCylinder(Core::Object& targetObj, Core::Engine& engine, float botRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, uint32 defautVertexInputTypeFlag = DefaultFlag);
			static void CreatePlane(Core::Object& targetObj,	Core::Engine& engine, float width, float depth, uint32 widthVertexCount, uint32 heightVertexCount, uint32 defautVertexInputTypeFlag = DefaultFlag);

		private:
			struct MeshInfo
			{
				uint32											stride;
				std::vector<Shader::VertexShader::SemanticInfo>	semantics;
			};
			static std::string UintToStr(uint32 data);
			static void AppendVertexData(std::vector<float>& inoutVertexDatas,
				const Math::Vector3& pos, const Math::Vector3& normal,
				const Math::Vector3& tangent, const Math::Vector2& uv, uint32 flag);
			static bool HasFlag(uint32 vtxInputTypeFlag, DefaultVertexInputTypeFlag flag);
			static void MakeMeshInfo(MeshInfo& outInfo, uint32 vtxInputTypeFlag);
		};
	}
}