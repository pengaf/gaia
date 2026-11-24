#include "native_file_system.h"
#include "native_file_system.mh"
#include "native_file_system.ic"
#include "native_file_system.mc"

BEGIN_GAIA

string_t NativeFileSystem::rootPath() const
{
	return m_rootPath.c_str();
}

NativeFileSystem& NativeFileSystem::rootPath(string_t rootPath)
{
	m_rootPath = rootPath.c_str();
	if (!m_rootPath.empty())
	{
		std::replace(m_rootPath.begin(), m_rootPath.end(), '\\', '/');
		if (m_rootPath.back() != '/')
		{
			m_rootPath += '/';
		}
	}
	return *this;
}

END_GAIA

