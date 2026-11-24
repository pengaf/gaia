#pragma once

#include "utility.h"
#include "slang/slang.h"
#include "slang/slang-com-ptr.h"
#include "slang/slang-com-helper.h"
#include <string>

BEGIN_GAIA

class ShaderCompiler
{
public:
	ShaderCompiler();
public:
	bool initialize(const char* profile, const char* rootPath);
	Slang::ComPtr<slang::IBlob> compileShader(const char* filePath, const char* entryPointName);
private:
	Slang::ComPtr<slang::IGlobalSession> m_globalSession{};
	Slang::ComPtr<slang::ISession> m_session{};
};

END_GAIA
