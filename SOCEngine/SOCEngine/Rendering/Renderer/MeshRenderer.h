#pragma once

#include "DefaultShaders.h"
#include "MeshManager.h"
#include "MaterialManager.h"

#include "MeshRenderQueue.h"

namespace Rendering
{
	namespace Renderer
	{
		class MeshRenderer final
		{
		public:
			struct Param
			{
				const Manager::BufferManager&	bufferMgr;
				const Manager::DefaultShaders&	defaultShaders;

				Param(const Manager::BufferManager& _bufferMgr, const Manager::DefaultShaders& _defaultShaders) 
					: bufferMgr(_bufferMgr), defaultShaders(_defaultShaders) {}
			};

			static void RenderWithoutIASetVB(Device::DirectX& dx, Param param, DefaultRenderType renderType, const Geometry::Mesh& mesh);

			template <class CallPreRender, class CallPostRender>
			static void RenderTransparentMeshes(Device::DirectX& dx, Param param, DefaultRenderType renderType, const Rendering::RenderQueue::TransparentMeshRenderQueue& meshes, CallPreRender preCall, CallPostRender postCall)
			{
				const auto& vbPool			= param.bufferMgr.GetPool<Buffer::VertexBuffer>();

				for (const auto meshPtr : meshes)
				{
					const auto* vertexBuffer	= vbPool.Find(meshPtr->GetVBKey()); assert(vertexBuffer);

					vertexBuffer->IASetBuffer(dx);

					preCall(meshPtr);
					RenderWithoutIASetVB(dx, param, renderType, *meshPtr);
					postCall();
				}
			}

			template <class CallPreRender, class CallPostRender>
			static void RenderOpaqueMeshes(Device::DirectX& dx, Param param, DefaultRenderType renderType, const Rendering::RenderQueue::OpaqueMeshRenderQueue& meshes, CallPreRender preRenderCall, CallPostRender postCall)
			{
				auto& vbPool				= param.bufferMgr.GetPool<Buffer::VertexBuffer>();
				const VertexBuffer* prevVB	= nullptr;				
				for (const auto meshPtr : meshes)
				{
					const VertexBuffer* curVB = vbPool.Find(mesh->GetVBKey());
					assert(curVB);
					
					if(curVB != prevVB)
					{
						prevVB = curVB;
						curVB->IASetBuffer(dx);
					}
		
					preRenderCall(mesh);
					RenderWithoutIASetVB(dx, param, renderType, *mesh);
					postCall();
				}
			}

			template <class CallPreRender, class CallPostRender>
			static void RenderAlphaTestMeshes(Device::DirectX& dx, Param param, DefaultRenderType renderType, const Rendering::RenderQueue::AlphaTestMeshRenderQueue& meshes, CallPreRender&& preCall, CallPostRender&& postCall)
			{
				RenderOpaqueMeshes(dx, param, renderType, meshes, preCall, postCall);
			}
		};
	}
}
