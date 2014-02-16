
#ifndef VPFILESYSTEM_H
#define VPFILESYSTEM_H

#include <fstream>

#include "VFSPP/core.hpp"

#include "globalincs/pstypes.h"
#include "cfile/VPFileSystemEntry.h"

#include <boost/unordered_map.hpp>

#include <boost/scoped_ptr.hpp>

#include <boost/filesystem.hpp>

#include <boost/thread/mutex.hpp>

namespace cfile
{
	using namespace vfspp;

	class VPFileData
	{
	public:
		std::string name;
		EntryType type;

		size_t offset;
		size_t size;
		time_t time;

		VPFileData() : name(), type(UNKNOWN), offset(0), size(0), time(0)
		{}
	};

	class VPFileSystem : public IFileSystem
	{
	private:
		std::vector<VPFileData> fileData;
		boost::unordered_map<string_type, size_t> indexMap;

		std::string rootPath;
		boost::filesystem::path filePath;

		boost::shared_ptr<std::streambuf> vpFileBuffer;
		std::istream vpStream;

		boost::scoped_ptr<VPFileSystemEntry> rootEntry;

		boost::mutex streamMutex;

		VPFileData getFileData(const string_type& path) const;

		void addFileData(VPFileData& data);

		friend class VPFileSystemEntry;

	public:
		VPFileSystem(const boost::filesystem::path& filePath, boost::shared_ptr<std::streambuf>& vpFileBuffer, const std::string& rootPath);

		virtual ~VPFileSystem();

		virtual VPFileSystemEntry* getRootEntry() { return rootEntry.get(); }

		virtual int supportedOperations() const { return vfspp::OP_READ; }
	};
}

#endif // VPFILESYSTEM_H
