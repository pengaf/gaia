#import "file_system.i"

#{
#include <string>
#}

namespace gaia
{
	class NativeFileSystem : FileSystem
	{
		string_t rootPath{ get set };
#{
	protected:
		std::string m_rootPath;//utf8
#}
	};
}