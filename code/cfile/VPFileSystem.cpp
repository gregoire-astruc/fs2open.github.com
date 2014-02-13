
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

	VPFileSystem::VPFileSystem(boost::shared_ptr<std::streambuf>& vpFileBufferIn, const std::string& rootPathIn)
		: vpFileBuffer(vpFileBufferIn), rootPath(rootPathIn), vpStream(vpFileBuffer.get())
	{
		Assert(vpFileBuffer);

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

				FileData data;
				data.name.assign(currentDir);
				data.type = DIRECTORY;
				
				data.offset = 0;
				data.size = 0;
				data.time = 0;

				fileData.push_back(data);
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

					FileData data;
					data.name.assign(currentDir);
					data.type = DIRECTORY;

					data.offset = 0;
					data.size = 0;
					data.time = 0;

					fileData.push_back(data);
				}
			}
			else
			{
				++numFiles;

				FileData data;
				data.type = vfspp::FILE;

				data.offset = file.offset;
				data.size = file.size;
				data.time = file.write_time;

				data.name.assign(currentDir);
				data.name.append(DirectorySeparatorStr);
				data.name.append(file.filename);

				fileData.push_back(data);
			}
		}

		rootEntry.reset(new VPFileSystemEntry(this, ""));

		mprintf(("%d files\n", numFiles));
	}

	VPFileSystem::~VPFileSystem()
	{
	}
}
