
#include "cfile/cfile.h"

#include "cfile/VPFileSystem.h"

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

namespace cfile
{
	struct VP_FILE_HEADER
	{
		char id[4];
		int version;
		int index_offset;
		int num_files;
	};

	struct VP_FILE
	{
		int	offset;
		int	size;
		char filename[32];
		_fs_time_t write_time;
	};

	VPFileSystem::VPFileSystem(const boost::filesystem::path& filePathIn, const std::string& rootPathIn)
		: rootPath(rootPathIn), vpStream(filePathIn, std::ios::binary), filePath(filePathIn)
	{
		VP_FILE_HEADER header;
		vpStream.read(reinterpret_cast<char*>(&header), sizeof(header));

		if (vpStream.gcount() != sizeof(header))
		{
			throw cfile::Exception("File is too small!");
		}

		if (header.id[0] != 'V' || header.id[1] != 'P' || header.id[2] != 'V' || header.id[3] != 'P')
		{
			throw cfile::Exception("File identifier is not valid!");
		}

		header.version = INTEL_INT(header.version); //-V570
		header.index_offset = INTEL_INT(header.index_offset); //-V570
		header.num_files = INTEL_INT(header.num_files); //-V570

		if (header.version > 2)
		{
			throw Exception("VP version is not supported!");
		}

		// Build the fileData vector

		std::string currentDir;

		// If we have a root path we pretend to contain the given path at the root level
		if (!rootPath.empty())
		{
			// normalize path
			rootPath = normalizePath(rootPath);

			std::vector<std::string> dirParts;

			boost::split(dirParts, rootPath, boost::is_any_of(DirectorySeparatorStr), boost::token_compress_on);

			BOOST_FOREACH(std::string& part, dirParts)
			{
				if (!currentDir.empty())
				{
					currentDir.append(DirectorySeparatorStr);
				}

				currentDir.append(part);

				VPFileData data;
				data.name.assign(currentDir);

				data.type = DIRECTORY;
				
				data.offset = 0;
				data.size = 0;
				data.time = 0;

				addFileData(data);
			}
		}

		vpStream.seekg(header.index_offset);
		int numFiles = 0;
		for (int i = 0; i < header.num_files; ++i)
		{
			VP_FILE file;

			vpStream.read(reinterpret_cast<char*>(&file), sizeof(VP_FILE));

			if (vpStream.gcount() != sizeof(VP_FILE))
			{
				throw EOFException();
			}

			file.offset = INTEL_INT(file.offset); //-V570
			file.size = INTEL_INT(file.size); //-V570
			file.write_time = INTEL_INT(file.write_time); //-V570

			if (file.size == 0)
			{
				if (!stricmp(file.filename, ".."))
				{
					// Backdir
					size_t slash = currentDir.find_last_of(DirectorySeparatorChar);

					if (slash != SCP_string::npos)
					{
						currentDir.resize(slash);
					}
					else
					{
						currentDir.resize(0);
					}
				}
				else
				{
					if (!currentDir.empty())
					{
						currentDir.append(DirectorySeparatorStr);
					}

					currentDir.append(file.filename);

					VPFileData data;
					data.name.assign(currentDir);

					data.type = DIRECTORY;

					data.offset = 0;
					data.size = 0;
					data.time = 0;

					addFileData(data);
				}
			}
			else
			{
				++numFiles;

				VPFileData data;
				data.type = vfspp::FILE;

				data.offset = file.offset;
				data.size = file.size;
				data.time = file.write_time;

				data.name.assign(currentDir);
				data.name.append(DirectorySeparatorStr);
				data.name.append(file.filename);

				addFileData(data);
			}
		}

		rootEntry.reset(new VPFileSystemEntry(this, ""));
	}

	VPFileSystem::~VPFileSystem()
	{
	}

	VPFileData VPFileSystem::getFileData(const string_type& path) const
	{
		if (path.length() == 0)
		{
			// Special case: The root entry is always a directory
			VPFileData data;
			data.type = DIRECTORY;

			return data;
		}

		boost::unordered_map<string_type, size_t>::const_iterator iter = indexMap.find(path);

		if (iter == indexMap.end())
		{
			return VPFileData();
		}
		else
		{
			return fileData[iter->second];
		}
	}

	void VPFileSystem::addFileData(VPFileData& data)
	{
		boost::to_lower(data.name);

		fileData.push_back(data);
		indexMap.insert(std::make_pair(data.name, fileData.size() - 1));
	}
}
