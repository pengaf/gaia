#{
#include "utility.h"
#include "../../../idlcpp2/paf/pafcore/memory.h"
#}

#import "object.i"
#import "blob.i"

namespace gaia
{
	class FileSystem : Object
	{
		virtual pafcore::Blob^ readFile(string_t name) #{ = 0#};
	};
}