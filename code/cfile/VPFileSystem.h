
#ifndef VPFILESYSTEM_H
#define VPFILESYSTEM_H

#include <fstream>

#include "VFSPP/core.hpp"

#include "globalincs/pstypes.h"
#include "cfile/VPFileSystemEntry.h"

#include <boost/scoped_ptr.hpp>

namespace cfile
{
	using namespace vfspp;

	class VPFileSystem : public IFileSystem
	{
	private:
		struct FileData
		{
			std::string name;
			EntryType type;

			size_t offset;
			size_t size;
			_fs_time_t time;
		};

		std::vector<FileData> fileData;

		std::string rootPath;

		boost::shared_ptr<std::streambuf> vpFileBuffer;
		std::istream vpStream;

		boost::scoped_ptr<VPFileSystemEntry> rootEntry;

		friend class VPFileSystemEntry;

	public:
		VPFileSystem(boost::shared_ptr<std::streambuf>& vpFileBuffer, const std::string& rootPath);

		virtual ~VPFileSystem();

		virtual VPFileSystemEntry* getRootEntry() { return rootEntry.get(); }

		virtual int supportedOperations() const { return vfspp::OP_READ; }
	};
}

#endif // VPFILESYSTEM_H
