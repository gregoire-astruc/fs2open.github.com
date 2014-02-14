
#include "cfile/VPFileSystem.h"
#include "cfile/VPFileSystemEntry.h"

#include <boost/foreach.hpp>

#include <boost/algorithm/string.hpp>

#include <boost/unordered_map.hpp>

#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

namespace cfile
{
	using namespace boost;
	using namespace boost::iostreams;

	class VPFileSource : public device<input_seekable>
	{
	public:
		std::streamsize begin;
		std::streamsize size;
		std::istream* vpStream;

		std::streamsize offset;

		VPFileSource() : begin(-1), size(-1), offset(0), vpStream(NULL)
		{}

		std::streamsize read(char* s, std::streamsize n)
		{
			// Read up to n characters from the input 
            // sequence into the buffer s, returning   
            // the number of characters read, or -1 
            // to indicate end-of-sequence.

			Assert(vpStream != NULL);

			std::streamsize currentPos = begin + offset;

			if (currentPos >= begin + size)
			{
				return -1;
			}

			std::streamsize remaining = size - offset;
			if (n > remaining)
			{
				n = remaining;
			}

			vpStream->seekg(currentPos, std::ios_base::beg);
			vpStream->read(s, n);

			offset += n;

			return vpStream->gcount();
		}

		std::streampos seek(stream_offset off, std::ios_base::seekdir way)
		{
			// Advances the read/write head by off characters, 
			// returning the new position, where the offset is 
			// calculated from:
			//  - the start of the sequence if way == ios_base::beg
			//  - the current position if way == ios_base::cur
			//  - the end of the sequence if way == ios_base::end

			Assert(vpStream != NULL);

			switch (way)
			{
			case std::ios::beg:
				offset = off;
				break;
			case std::ios::cur:
				offset += off;
				break;
			case std::ios::end:
				offset = size + off;
				break;
			default:
				Int3();
				return -1;
			}

			return offset;
		}
	};

	VPFileSystemEntry::VPFileSystemEntry(VPFileSystem* parentSystem, const string_type& path)
		: IFileSystemEntry(path), parentSystem(parentSystem)
	{
	}

	FileEntryPointer VPFileSystemEntry::getChild(const string_type& path)
	{
		if (getType() != DIRECTORY)
		{
			throw InvalidOperationException("Entry is no directory!");
		}

		string_type childPath = this->path;
		childPath.append(DirectorySeparatorStr).append(path);
		childPath = normalizePath(childPath);

		EntryType type = getFileData(childPath).type;

		if (type == UNKNOWN)
		{
			return FileEntryPointer();
		}
		else
		{
			return FileEntryPointer(new VPFileSystemEntry(parentSystem, childPath));
		}
	}

	size_t VPFileSystemEntry::numChildren()
	{
		if (getType() != DIRECTORY)
		{
			throw InvalidOperationException("Entry is no directory!");
		}

		size_t num = 0;
		BOOST_FOREACH(const VPFileData& data, parentSystem->fileData)
		{
			if (boost::algorithm::starts_with(data.name, path))
			{
				// This is a child of this directory, now we need to check if it's in a subdirectory
				size_t pos = data.name.find_last_of('/');

				if (pos == string_type::npos)
				{
					// Couldn't find separator, it's a child if we are the root
					if (path.length() == 0)
					{
						++num;
					}
				}
				else
				{
					if (pos == path.length())
					{
						++num;
					}
				}
			}
		}

		return num;
	}

	void VPFileSystemEntry::listChildren(std::vector<FileEntryPointer>& outVector)
	{
		if (getType() != DIRECTORY)
		{
			throw InvalidOperationException("Entry is no directory!");
		}

		outVector.clear();

		BOOST_FOREACH(const VPFileData& data, parentSystem->fileData)
		{
			if (boost::algorithm::starts_with(data.name, path))
			{
				// This is a child of this directory, now we need to check if it's in a subdirectory
				size_t pos = data.name.find_last_of('/');

				if (pos == string_type::npos)
				{
					// Couldn't find separator, it's a child if we are the root
					if (path.length() == 0)
					{
						outVector.push_back(FileEntryPointer(new VPFileSystemEntry(parentSystem, data.name)));
					}
				}
				else
				{
					if (pos == path.length())
					{
						outVector.push_back(FileEntryPointer(new VPFileSystemEntry(parentSystem, data.name)));
					}
				}
			}
		}
	}

	EntryType VPFileSystemEntry::getType() const
	{
		return getFileData(path).type;
	}

	VPFileData VPFileSystemEntry::getFileData(const string_type& path) const
	{
		if (path.length() == 0)
		{
			// Special case: The root entry is always a directory
			VPFileData data;
			data.type = DIRECTORY;

			return data;
		}

		std::vector<VPFileData>::const_iterator iter;
		for (iter = parentSystem->fileData.begin(); iter != parentSystem->fileData.end(); ++iter)
		{
			if (boost::algorithm::iequals(iter->name, path))
			{
				// Only compare case insensitive
				return *iter;
			}
		}

		return VPFileData();
	}

	bool VPFileSystemEntry::deleteChild(const string_type& name)
	{
		throw InvalidOperationException("VP archives are read only!");
	}

	FileEntryPointer VPFileSystemEntry::createEntry(EntryType type, const string_type& name)
	{
		throw InvalidOperationException("VP archives are read only!");
	}

	boost::shared_ptr<std::streambuf> VPFileSystemEntry::open(int mode)
	{
		if (getType() != vfspp::FILE)
		{
			throw InvalidOperationException("Entry is no file!");
		}

		if (mode & MODE_WRITE)
		{
			throw InvalidOperationException("VP archives are read only!");
		}

		VPFileData data;
		std::vector<VPFileData>::const_iterator iter;
		for (iter = parentSystem->fileData.begin(); iter != parentSystem->fileData.end(); ++iter)
		{
			// Only compare case insensitive
			if (boost::algorithm::iequals(iter->name, path))
			{
				data = *iter;
			}
		}

		if (mode & MODE_MEMORY_MAPPED)
		{
			throw FileSystemException("Cannot open VP-entry in memory mapped mode!");
		}

		typedef stream_buffer<VPFileSource> vpBuffer;

		VPFileSource source;
		source.begin = data.offset;
		source.size = data.size;
		source.vpStream = &parentSystem->vpStream;

		shared_ptr<vpBuffer> buffer = shared_ptr<vpBuffer>(new vpBuffer(source));

		if (!buffer->is_open())
		{
			throw FileSystemException("Failed to open memory mapped file!");
		}
		else
		{
			return buffer;
		}
	}

	void VPFileSystemEntry::rename(const string_type& newPath)
	{
		throw InvalidOperationException("VP archives are read-only!");
	}

	time_t VPFileSystemEntry::lastWriteTime()
	{
		return getFileData(path).time;
	}
}
