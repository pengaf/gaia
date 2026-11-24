#include "shader_compiler.h"

BEGIN_GAIA

ShaderCompiler::ShaderCompiler()
{}

bool ShaderCompiler::initialize(const char* profile, const char* rootPath)
{
    SlangResult result;
    result = slang::createGlobalSession(m_globalSession.writeRef());
    if(SLANG_FAILED(result))
    {
        return false;
    }

    slang::TargetDesc targetDesc = {};
    targetDesc.format = SLANG_SPIRV;
    targetDesc.profile = m_globalSession->findProfile(profile);
    targetDesc.flags = 0;//default flags deprecated

    slang::SessionDesc sessionDesc = {};
	sessionDesc.targets = &targetDesc;
	sessionDesc.targetCount = 1;

    result = m_globalSession->createSession(sessionDesc, m_session.writeRef());
    if(SLANG_FAILED(result))
    {
        return false;
    }
    return true;
}

Slang::ComPtr<slang::IBlob> ShaderCompiler::compileShader(const char* filePath, const char* entryPointName)
{
    SlangResult result;
    Slang::ComPtr<slang::IBlob> diagnostics;
    slang::IModule* module = m_session->loadModule(filePath, diagnostics.writeRef());

    if (!module)
    {
		return nullptr;
    }

    Slang::ComPtr<slang::IEntryPoint> entryPoint;
    result =  module->findEntryPointByName(entryPointName, entryPoint.writeRef());
    if (SLANG_FAILED(result))
    {
        return nullptr;
    }


    slang::IComponentType* components[] = { module, entryPoint.get() };
    Slang::ComPtr<slang::IComponentType> composite;
    result = m_session->createCompositeComponentType(components, 2, composite.writeRef(), diagnostics.writeRef());

    if (SLANG_FAILED(result))
    {
        return nullptr;
    }

    Slang::ComPtr<slang::IBlob> outCode;
    result = composite->getEntryPointCode(0, 0, outCode.writeRef(), diagnostics.writeRef());

    if (SLANG_FAILED(result))
    {
        return nullptr;
    }

    return outCode;
}

END_GAIA
