#include "../../gaia/graphics/shader_compiler.h"
#include <iostream>

int main()
{
	gaia::ShaderCompiler compiler;
    bool initialized = compiler.initialize();
    if (!initialized)
    {
        std::cerr << "Failed to initialize the shader compiler.\n";
        return 1;
    }
    return 0;
}
