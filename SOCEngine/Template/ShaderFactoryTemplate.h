#pragma once

#include <ShaderManager.h>

class ShaderFactory
{
private:
	Rendering::Shader::ShaderManager	*_shaderMgr;

public:
	ShaderFactory(Rendering::Shader::ShaderManager*& shaderManager)
	{
		_shaderMgr = shaderManager;
	}

	~ShaderFactory(void)
	{
	}

public:
	bool LoadShader(const std::string& shaderName,
		const std::string& mainVSFuncName, const std::string& mainPSFuncName,
		Rendering::Shader::VertexShader*& outVertexShader,
		Rendering::Shader::PixelShader*& outPixelShader)
	{
		std::string folderPath = "";
		std::vector<D3D11_INPUT_ELEMENT_DESC> vertexDeclations;
		D3D11_INPUT_ELEMENT_DESC desc;
		auto AddInputElementDesc = [&](const std::string& semanticName, uint semanticIndex, DXGI_FORMAT format, uint alignedByteOffset, D3D11_INPUT_CLASSIFICATION inputSlotClass, uint inputSlot, uint instanceDataStepRate)
		{
			typedef unsigned int uint;
			auto MakeInputElementDesc = [&](D3D11_INPUT_ELEMENT_DESC& out)
			{
				out.SemanticName = semanticName;
				out.SemanticIndex = semanticIndex;
				out.AlignedByteOffset = alignedByteOffset;
				out.Format = format;
				out.InputSlotClass = inputSlotClass;
				out.InputSlot = inputSlot;
				out.InstanceDataStepRate = instanceDataStepRate;
			}
		
			D3D11_INPUT_ELEMENT_DESC desc;
			MakeInputElementDesc(desc);
			vertexDeclations.push_back(desc);
		};
		/** Script Begin **/
		/** Script End **/
		const std::string baseCommand = shaderName+":";
		outVertexShader = dynamic_cast<Rendering::Shader::VertexShader*>(_shaderMgr->LoadVertexShader(folderPath, baseCommand + mainVSFuncName, true, vertexDeclations));
		outPixelShader	= dynamic_cast<Rendering::Shader::PixelShader*>(_shaderMgr->LoadPixelShader(folderPath, baseCommand + mainPSFuncName, false));
		return false;
	}
};

