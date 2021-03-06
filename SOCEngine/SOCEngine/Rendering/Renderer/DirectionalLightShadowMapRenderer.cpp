#include "DirectionalLightShadowMapRenderer.h"
#include "ShadowManager.h"

#include "VertexShader.h"
#include "AutoBinder.hpp"

using namespace Rendering::Renderer::ShadowMap;
using namespace Rendering::Renderer;
using namespace Rendering::Manager;
using namespace Rendering::Shadow;
using namespace Rendering::Shader;
using namespace Rendering::Texture;
using namespace Rendering::Buffer;
using namespace Rendering::Geometry;
using namespace Rendering::RenderState;
using namespace Rendering::Material;
using namespace Device;
using namespace Core;

void DirectionalLightShadowMapRenderer::Render(DirectX& dx, Param&& param,
	const TempRenderQueue& renderQ, const MeshRenderer::Param& meshParam,
	const ConstBuffersType& shadowMapVPMatCB)
{
	float mapResolution	= static_cast<float>(param.atlasMapResolution);
	auto viewport		= Rect<float>(mapResolution * float(param.shadowIndex), 0.0f, mapResolution, mapResolution);

	dx.SetViewport(viewport);
	dx.SetRenderTarget(*param.shadowMap.GetViewDepthMap(), param.shadowMap);
	dx.SetDepthStencilState(DepthState::Greater, 0);
	dx.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
	dx.SetBlendState(BlendState::Opaque);

	param.shadowMap.Clear(dx, 0.0f, 0u);

	AutoBinderSampler<PixelShader> defaultSampler(dx,	SamplerStateBindIndex::DefaultSamplerState,		SamplerState::Linear);
	AutoBinderCB<VertexShader> viewProjMatCB(dx,		ConstBufferBindIndex::OnlyPassViewProjMat,		shadowMapVPMatCB.viewProjMatCB);
	AutoBinderCB<VertexShader> viewMatCB(dx,			ConstBufferBindIndex::PCSSViewDepth,			shadowMapVPMatCB.viewMatCB);

	dx.SetRasterizerState(RasterizerState::CWDefault);
	MeshRenderer::RenderOpaqueMeshes(dx, meshParam, DefaultRenderType::Forward_PCSSViewDepth, renderQ.opaqueRenderQ, [](const Mesh* mesh){ }, [](){});

	dx.SetRasterizerState(RasterizerState::CWDisableCulling);
	MeshRenderer::RenderAlphaTestMeshes(dx, meshParam, DefaultRenderType::Forward_PCSSViewDepthAlphaTest, renderQ.alphaTestRenderQ,
		[&dx, &materialMgr = param.materialMgr](const Mesh* mesh)
		{
			auto material	= materialMgr.Find<PhysicallyBasedMaterial>(mesh->GetPBRMaterialID()); assert(material);

			auto diffuseMap	= material->GetTextures().Find(PhysicallyBasedMaterial::GetDiffuseMapKey()); assert(diffuseMap);
			PixelShader::BindShaderResourceView(dx, TextureBindIndex::DiffuseMap, diffuseMap->resource.GetShaderResourceView());

			auto opacityMap = material->GetTextures().Find(PhysicallyBasedMaterial::GetOpacityMapKey());
			if (opacityMap)
				PixelShader::BindShaderResourceView(dx, TextureBindIndex::OpacityMap, opacityMap->resource.GetShaderResourceView());
		},
		[&dx]()
		{
			PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::OpacityMap);
			PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::DiffuseMap);
		}
	);

	dx.SetRasterizerState(RasterizerState::CWDefault);
	dx.ReSetRenderTargets(1);
}
