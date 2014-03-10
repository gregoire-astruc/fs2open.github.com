/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/ 

#include "globalincs/pstypes.h"

#include "cfile/cfile.h"

#include "cfile/VPFileSystem.h"
#include "cfile/ZipFileSystem.h"

#include "cmdline/cmdline.h"

#include "parse/encrypt.h"
#include "osapi/osapi.h"

#include <VFSPP/core.hpp>
#include <VFSPP/merged.hpp>
#include <VFSPP/system.hpp>
#include <VFSPP/7zip.hpp>

#include <boost/filesystem.hpp>

#include <boost/smart_ptr.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>

#include <boost/bind.hpp>
#include <boost/ref.hpp>

#include <unzip.h>

namespace cfile
{
	using namespace vfspp;
	using namespace vfspp::merged;
	using namespace vfspp::system;

	using namespace boost;
	namespace fs = filesystem;
	
	struct FileHandle
	{
		shared_ptr<std::streambuf> buffer;

		FileEntryPointer entry;

		int mode;

		std::iostream stream;

		size_t maxReadLength;
		std::streampos readLengthStart;
		std::streamoff readLengthOffset;

		FileHandle(shared_ptr<std::streambuf> bufferIn, FileEntryPointer entryIn, int modeIn)
			: buffer(bufferIn), stream(bufferIn.get()), entry(entryIn), mode(modeIn), maxReadLength(0)
		{
			Assert(buffer);
			Assert(entryIn);
		}
	};

	fs::path rootDir;

	fs::path userDir;

	shared_ptr<object_pool<FileHandle> > cfilePool;

	shared_ptr<MergedFileSystem> fileSystem;

	bool inited = false;

	// During cfile_init, verify that Pathtypes[n].index == n for each item
	// Each path must have a valid parent that can be tracable all the way back to the root 
	// so that we can create directories when we need to.
	//
	// Please make sure extensions are all lower-case, or we'll break unix compatibility
	//
	const char* Pathtypes[MAX_PATH_TYPES]  = {
		//Path
		{ NULL },
		// Root must be index 1!!
		{ "" },
		{ "data" },
		{ "data/maps" },
		{ "data/text" },
		{ "data/models" },
		{ "data/tables" },
		{ "data/sounds" },
		{ "data/sounds/8b22k" },
		{ "data/sounds/16b11k" },
		{ "data/voice" },
		{ "data/voice/briefing" },
		{ "data/voice/command_briefings" },
		{ "data/voice/debriefing" },
		{ "data/voice/personas" },
		{ "data/voice/special" },
		{ "data/voice/training" },
		{ "data/music" },
		{ "data/movies" },
		{ "data/interface" },
		{ "data/fonts" },
		{ "data/effects" },
		{ "data/hud" },
		{ "data/players" },
		{ "data/players/images" },
		{ "data/players/squads" },
		{ "data/players/single" },
		{ "data/players/single/inferno" },
		{ "data/players/multi" },
		{ "data/players/multi/inferno" },
		{ "data/cache" },
		{ "data/multidata" },
		{ "data/missions" },
		{ "data/config" },
		{ "data/demos" },
		{ "data/cbanims" },
		{ "data/intelanims" },
		{ "data/scripts" },
		{ "data/fiction" },
	};

	void addModDirs(SCP_vector<fs::path>& rootDirs, const fs::path& rootDir)
	{
		if (Cmdline_mod) {
			const char* cur_pos;

			// stackable Mod support -- Kazan
			for (cur_pos = Cmdline_mod; *cur_pos != '\0'; cur_pos += (strlen(cur_pos) + 1))
			{
				fs::path modRoot = rootDir / cur_pos;

				if (fs::exists(modRoot))
				{
					rootDirs.push_back(modRoot);
				}
			}
		}
	}

	void searchRootDirectories(SCP_vector<fs::path>& rootDirs, const char* cdromDirStr)
	{
		// First init root directory
		rootDir = fs::current_path();

#ifdef SCP_UNIX
		userDir.assign(detect_home());
		userDir /= Osreg_user_dir;
#endif

		if (!userDir.empty())
		{
			addModDirs(rootDirs, userDir);

			rootDirs.push_back(userDir);
		}

		addModDirs(rootDirs, rootDir);

		rootDirs.push_back(rootDir);

		if (cdromDirStr != NULL)
		{
			rootDirs.push_back(fs::path(cdromDirStr));
		}
	}

	void maybeAddPackSystem(fs::directory_entry entry, SCP_vector<IFileSystem*>& fileSystems)
	{
		if (entry.path().has_extension())
		{
			if (boost::iequals(entry.path().extension().string(), ".vp"))
			{
				// Standard VPs
				try
				{
					mprintf(("Found root pack '%s' ... \n", entry.path().string().c_str()));

					VPFileSystem* system = new VPFileSystem(entry.path(), "");
					fileSystems.push_back(system);
				}
				catch (std::exception& e)
				{
					Error(LOCATION, "Error while reading file %s: %s\n", entry.path().filename().string().c_str(), e.what());
				}
			}
			else if (boost::iequals(entry.path().extension().string(), ".vp7"))
			{
				// 7-zip archive
				try
				{
					mprintf(("Found root pack '%s' ... \n", entry.path().string().c_str()));

					sevenzip::SevenZipFileSystem* system = new sevenzip::SevenZipFileSystem(entry.path());
					fileSystems.push_back(system);
				}
				catch (std::exception& e)
				{
					Error(LOCATION, "Error while reading file %s: %s\n", entry.path().filename().string().c_str(), e.what());
				}
			}
			else if (boost::iequals(entry.path().extension().string(), ".vpz"))
			{
				// Zip-archive
				try
				{
					mprintf(("Found root pack '%s' ... \n", entry.path().string().c_str()));

					ZipFileSystem* system = new ZipFileSystem(entry.path());
					fileSystems.push_back(system);
				}
				catch (std::exception& e)
				{
					Error(LOCATION, "Error while reading file %s: %s\n", entry.path().filename().string().c_str(), e.what());
				}
			}
		}
	}

	void initializeFileSystems(SCP_vector<IFileSystem*>& fileSytems, const SCP_vector<fs::path>& rootDirs)
	{
		bool haveUserDir = !userDir.empty();

		// Initialize physical file systems
		for (const fs::path& path : rootDirs)
		{
			if (!fs::exists(path))
			{
				mprintf(("Was prompted to add root '%s' but it doesn't exist, skiping...", path.string().c_str()));
				continue;
			}

			PhysicalFileSystem* pathSystem = new PhysicalFileSystem(path);

			if (haveUserDir)
			{
				// If we have a user dir then direct all write operations there
				bool isUserDir = fs::equivalent(path, userDir);

				if (isUserDir)
				{
					pathSystem->setAllowedOperations(OP_READ | OP_WRITE | OP_CREATE | OP_DELETE);
				}
				else
				{
					// Only allow read operations anywhere else for now
					pathSystem->setAllowedOperations(OP_READ);
				}
			}
			else
			{
				// If we have no user dir then use the root for writing
				bool isRootDir = fs::equivalent(path, rootDir);

				if (isRootDir)
				{
					pathSystem->setAllowedOperations(OP_READ | OP_WRITE | OP_CREATE | OP_DELETE);
				}
				else
				{
					// Only allow read operations anywhere else for now
					pathSystem->setAllowedOperations(OP_READ);
				}
			}

			fileSytems.push_back(pathSystem);
		}
		
		fs::directory_iterator endIter;
		// Now go through every root and search for pack files
		for (const fs::path& path : rootDirs)
		{
			fs::directory_iterator dirIter(path);

			std::for_each(dirIter, endIter, boost::bind(maybeAddPackSystem, _1, boost::ref(fileSytems)));
		}
	}

	bool init(const char* cdromDirStr)
	{
		if (inited)
		{
			return true;
		}

		inited = true;

		SCP_vector<fs::path> rootDirs;
		searchRootDirectories(rootDirs, cdromDirStr);

#ifndef NDEBUG
		// Print the roots we use in debug mode
		for (const fs::path& path : rootDirs)
		{
			mprintf(("Found root %s... \n", path.string().c_str()));
		}
#endif

		SCP_vector<IFileSystem*> fileSystems;
		initializeFileSystems(fileSystems, rootDirs);

		// initialize encryption
		encrypt_init();

		// Construct first file system to search for pack files
		fileSystem.reset(new MergedFileSystem());
		fileSystem->setCaseInsensitive(true);

		for (IFileSystem* system : fileSystems)
		{
			fileSystem->addFileSystem(system);
		}

		fileSystem->populateEntries(1);

		cfilePool.reset(new boost::object_pool<FileHandle>());

		checksum::crc::init();

		return true;
	}

	void shutdown()
	{
		cfilePool.reset();

		fileSystem.reset();
	}

	SCP_string getRootDir()
	{
		std::string str = rootDir.string();

		return SCP_string(str.begin(), str.end());
	}

	bool access(const SCP_string& file, cfile::DirType type, int mode)
	{		
		fs::path workPath(rootDir);
		workPath /= Pathtypes[type];
		workPath /= std::string(file.begin(), file.end());

		fs::file_status fileStatus = fs::status(workPath);

		return fileStatus.permissions() == mode;
	}

	inline bool dirTypeValid(DirType type)
	{
		return type > TYPE_INVALID && type < MAX_PATH_TYPES;
	}

	FileEntryPointer getEntry(const SCP_string& path, int mode, bool localize)
	{
		FileEntryPointer entry = fileSystem->getRootEntry()->getChild(path.c_str());

		if (entry && entry->getType() == DIRECTORY)
		{
			return FileEntryPointer();
		}
		else
		{
			if (!entry && mode & MODE_WRITE)
			{
				entry = fileSystem->getRootEntry()->createEntry(vfspp::FILE, path.c_str());
			}

			return entry;
		}
	}

	FileEntryPointer getFileEntry(const SCP_string& path, int mode, DirType dir_type, bool localize)
	{
		if (path.find(DirectorySeparatorChar) != SCP_string::npos)
		{
			// We already have a path
			return getEntry(path, mode, localize);
		}

		if (dir_type == TYPE_ANY)
		{
			SCP_string searchPath;

			FileEntryPointer entry;
			// Start at 1 because 0 is the invalid entry
			for (int i = 1; i < MAX_PATH_TYPES; ++i)
			{
				searchPath.assign(Pathtypes[i]);
				searchPath.append(DirectorySeparatorStr);
				searchPath.append(path);

				entry = getEntry(searchPath, mode, localize);

				// Break once we have found a valid entry
				if (entry)
					break;
			}

			return entry;
		}
		else
		{
			Assert(dirTypeValid(dir_type));

			SCP_stringstream stream;

			stream << Pathtypes[dir_type] << DirectorySeparatorChar << path;

			return getEntry(stream.str(), mode, localize);
		}
	}

	FileEntryPointer getParent(const FileEntryPointer& child)
	{
		Assertion(child, "Invalid child pointer given!");

		std::string path = child->getPath();

		size_t slash = path.find_last_of(DirectorySeparatorChar);

		if (slash == std::string::npos)
		{
			return FileEntryPointer();
		}
		else
		{
			return fileSystem->getRootEntry()->getChild(path.substr(0, slash));
		}
	}

	struct NameSorter
	{
		bool operator()(const FileEntryPointer& entryA, const FileEntryPointer& entryB) const
		{
			return entryA->getPath() < entryB->getPath();
		}
	};

	struct TimeSorter
	{
		bool operator()(const FileEntryPointer& entryA, const FileEntryPointer& entryB) const
		{
			return entryA->lastWriteTime() < entryB->lastWriteTime();
		}
	};

	void sortEntryVector(std::vector<FileEntryPointer>& children, SortMode mode)
	{
		switch (mode)
		{
		case SORT_NAME:
			std::sort(children.begin(), children.end(), NameSorter());
			break;
		case SORT_TIME:
			std::sort(children.begin(), children.end(), TimeSorter());
			std::reverse(children.begin(), children.end()); // Revese so we have a descending order
			break;
		case SORT_REVERSE:
			std::sort(children.begin(), children.end(), NameSorter());
			std::reverse(children.begin(), children.end());
			break;
		default:
			Error(LOCATION, "Invalid sort mode! Get a coder!");
		}
	}

	SCP_string getEntryFileName(FileEntryPointer& entry)
	{
		SCP_string out;

		const std::string& path = entry->getPath();

		size_t slash = path.find_last_of(DirectorySeparatorChar);

		if (slash == std::string::npos)
		{
			out.assign(path.begin(), path.end());
		}
		else
		{
			out.assign(path.begin() + slash + 1, path.end());
		}

		return out;
	}

	SCP_string getEntryPath(FileEntryPointer& entry)
	{
		const std::string& path = entry->getPath();

		return SCP_string(path.begin(), path.end());
	}

	bool shouldRemoveFull(FileEntryPointer& entryPointer, const boost::regex& wildcardMatcher, ListFilterFunction filterFunc)
	{
		if (entryPointer->getType() != vfspp::FILE)
		{
			return false;
		}

		const string_type& path = entryPointer->getPath();

		// as string::npos is (size_t)-1, using + 1 will make it overflow to 0
		size_t begin = path.find_last_of(DirectorySeparatorStr) + 1;

		bool keep = boost::regex_match(path.begin() + begin, path.end(), wildcardMatcher);

		if (keep && filterFunc != NULL)
		{
			keep = filterFunc(path.substr(begin));
		}

		return !keep;
	}

	bool shouldRemoveFunc(FileEntryPointer& entryPointer, ListFilterFunction filterFunc)
	{
		if (entryPointer->getType() != vfspp::FILE)
		{
			return false;
		}

		const string_type& path = entryPointer->getPath();

		// as string::npos is (size_t)-1, using + 1 will make it overflow to 0
		size_t begin = path.find_last_of(DirectorySeparatorStr) + 1;

		return filterFunc(path.substr(begin));
	}

	bool shouldRemoveQuick(FileEntryPointer& entryPointer)
	{
		return entryPointer->getType() != vfspp::FILE;
	}

	void filterFiles(std::vector<FileEntryPointer>& children, const SCP_string& filter, ListFilterFunction filterFunc)
	{
		if (!filter.empty())
		{
			// RegExp magic to escape otherwise invalid characters
			const boost::regex esc("([\\^\\.\\$\\|\\(\\)\\[\\]\\*\\+\\?\\/\\\\])");
			const char* replacement = "\\\\\\1";
			std::string filterResult = boost::regex_replace(std::string(filter.begin(), filter.end()), esc,
				replacement, boost::match_default | boost::format_sed);

			// We also escaped our wildcards so we need to restore those
			boost::replace_all(filterResult, "\\*", ".*");
			boost::replace_all(filterResult, "\\?", ".");

			boost::regex wildcardMatcher(filterResult);

			children.erase(std::remove_if(children.begin(), children.end(),
				boost::bind(shouldRemoveFull, _1, boost::cref(wildcardMatcher), filterFunc)), children.end());
		}
		else if (filterFunc != NULL)
		{
			children.erase(std::remove_if(children.begin(), children.end(), boost::bind(shouldRemoveFunc, _1, filterFunc)), children.end());
		}
		else
		{
			// Only remove non-files
			children.erase(std::remove_if(children.begin(), children.end(), boost::bind(shouldRemoveQuick, _1)), children.end());
		}
	}

	void listFiles(SCP_vector<SCP_string>& names, DirType pathType, const SCP_string& filter, SortMode sortMode, ListFilterFunction filterFunc, bool returnFullPath)
	{
		Assert(dirTypeValid(pathType));

		listFiles(names, Pathtypes[pathType], filter, sortMode, filterFunc, returnFullPath);
	}

	void listFiles(SCP_vector<SCP_string>& names, const SCP_string& path, const SCP_string& filter, SortMode sortMode, ListFilterFunction filterFunc, bool returnFullPath)
	{
		names.clear();

		FileEntryPointer parentDir = fileSystem->getRootEntry()->getChild(path.c_str());

		if (!parentDir)
		{
			// Directory not present, no children to be listed...
			return;
		}

		std::vector<FileEntryPointer> children;

		parentDir->listChildren(children);

		filterFiles(children, filter, filterFunc);

		if (sortMode != SORT_NONE)
		{
			sortEntryVector(children, sortMode);
		}

		names.resize(children.size());

		if (returnFullPath)
		{
			std::transform(children.begin(), children.end(), names.begin(), getEntryPath);
		}
		else
		{
			std::transform(children.begin(), children.end(), names.begin(), getEntryFileName);
		}
	}

	bool deleteFile(const SCP_string& path, DirType type, bool localize)
	{
		FileEntryPointer entry = getFileEntry(path, MODE_READ, type, localize);

		if (entry)
		{
			return fileSystem->getRootEntry()->deleteChild(entry->getPath());
		}
		else
		{
			return true;
		}
	}

	size_t flushDir(DirType type)
	{
		Assert(dirTypeValid(type));

		FileEntryPointer entry = fileSystem->getRootEntry()->getChild(Pathtypes[type]);

		if (!entry)
		{
			return 0;
		}

		IFileSystemEntry *root = fileSystem->getRootEntry();

		std::vector<FileEntryPointer> children;
		entry->listChildren(children);

		size_t num = 0;

		std::vector<FileEntryPointer>::iterator iter;
		for (iter = children.begin(); iter != children.end(); ++iter)
		{
			if ((*iter)->getType() == vfspp::FILE)
			{
				if (root->deleteChild((*iter)->getPath()))
				{
					++num;
				}
			}
		}

		return num;
	}

	DirType getPathType(const SCP_string& path)
	{
		SCP_string lowerPath = boost::to_lower_copy(path);

		int type = std::distance(Pathtypes, std::find(Pathtypes, Pathtypes + MAX_PATH_TYPES, lowerPath.c_str()));

		if (type == MAX_PATH_TYPES)
		{
			return TYPE_INVALID;
		}
		else
		{
			return static_cast<DirType>(type);
		}
	}

	bool rename(const SCP_string& oldName, const SCP_string& newName, DirType type, bool localize)
	{
		Assert(dirTypeValid(type));

		FileEntryPointer entry = getFileEntry(oldName, MODE_READ, type, localize);

		try
		{
			entry->rename(std::string(Pathtypes[type]).append(DirectorySeparatorStr).append(newName.begin(), newName.end()));
			
			return true;
		} 
		catch (...)
		{
			return false;
		}
	}

	bool findFile(const SCP_string& name, SCP_string& outName, DirType type, const char** exts, size_t numExts, size_t* out_extIndex, bool localize)
	{
		if (exts == NULL)
		{
			FileEntryPointer entry = getFileEntry(name, MODE_READ, type, localize);

			if (out_extIndex)
				*out_extIndex = (size_t)-1; // Intentional overflow

			if (entry)
			{
				outName.assign(entry->getPath().c_str());

				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			SCP_string correctName(name);

			size_t dot = correctName.find_last_of(".");

			if (dot != SCP_string::npos)
			{
				// We may only strip the extension if it is longer than 1 character
				if (correctName.size() - dot > 2)
				{
					correctName.resize(dot);
				}
			}

			SCP_string workString;

			for (size_t i = 0; i < numExts; ++i)
			{
				Assertion(strlen(exts[i]) > 3, "It is assumed that any extension is at least 3 characters long, get a coder!");

				workString.assign(correctName);
				workString.append(exts[i]);

				FileEntryPointer entry = getFileEntry(workString, MODE_READ, type, localize);

				if (entry)
				{
					if (out_extIndex)
						*out_extIndex = i;

					outName.assign(entry->getPath().begin(), entry->getPath().end());


					return true;
				}
			}

			if (out_extIndex)
				*out_extIndex = (size_t)-1; // Intentional overflow

			// We haven't found anything...
			return false;
		}
	}

	bool exists(const SCP_string& path, DirType type, bool localize)
	{
		return (bool)getFileEntry(path, MODE_READ, type, localize);
	}

	int toVfsppMode(int mode, OpenType type)
	{
		int outMode = 0;

		if (mode & MODE_READ)
		{
			outMode |= IFileSystemEntry::MODE_READ;
		}

		if (mode & MODE_WRITE)
		{
			outMode |= IFileSystemEntry::MODE_WRITE;
		}

		if (type & OPEN_MEMORY_MAPPED)
		{
			outMode |= IFileSystemEntry::MODE_MEMORY_MAPPED;
		}

		Assertion(outMode != 0, "Invalid open mode specified!");

		return outMode;
	}

	FileHandle* open(const SCP_string& file_path, int mode, OpenType type, DirType dir_type, bool localize)
	{
		Assertion(inited, "CFile system has not been inited!");

		Assertion(!file_path.empty(), "Invalid file path given!");
		Assertion(mode != 0, "Invalid mode given!");

		FileEntryPointer fullEntry = getFileEntry(file_path, mode, dir_type, localize);

		if (!fullEntry)
		{
			return NULL;
		}

		try
		{
			boost::shared_ptr<std::streambuf> buffer = fullEntry->open(toVfsppMode(mode, type));

			if (!buffer)
			{
				return NULL;
			}

			FileHandle* handle = cfilePool->construct(buffer, fullEntry, mode);

			Assertion(handle, "Failed to allocate memory for file handle!");

			return handle;
		}
		catch (const FileSystemException& e)
		{
			mprintf(("Failed to open %s: %s\n", file_path.c_str(), e.what()));
		}
		catch (const InvalidOperationException& e)
		{
			mprintf(("Failed to open %s: %s\n", file_path.c_str(), e.what()));
		}

		return NULL;
	}

	bool close(FileHandle* handle)
	{
		Assert(handle);
		Assertion(cfilePool->is_from(handle), "Handle not allocated by the cfile system!");

		handle->stream.flush();
		try
		{
			cfilePool->destroy(handle);
		}
		catch (...)
		{
			// in case the destructor throws, catch and return false
			return false;
		}

		return true;
	}

	std::iostream& getStream(FileHandle* handle)
	{
		Assert(handle != NULL);
		Assertion(handle->maxReadLength == 0, "Trying to use C++ stream with read length restriction is not possible!");

		return handle->stream;
	}

	void setMaxReadLength(FileHandle* handle, size_t size)
	{
		Assert(handle != NULL);
		Assert(handle->mode & MODE_READ);

		handle->maxReadLength = size;
		handle->readLengthOffset = 0;
		handle->readLengthStart = handle->stream.tellg();
	}

	const std::string& getFilePath(FileHandle* handle)
	{
		return handle->entry->getPath();
	}

	bool flush(FileHandle* handle)
	{
		Assert(handle != NULL);

		try
		{
			handle->stream.flush();
		}
		catch (std::exception& e)
		{
			mprintf(("Failed to flush stream: %s", e.what()));
			return false;
		}
		catch (...)
		{
			mprintf(("Failed to flush stream!"));
			return false;
		}

		return true;
	}

	int seek(FileHandle *fp, int offset, cfile::SeekMode where)
	{
		Assert(fp != NULL);

		std::ios::seekdir dir;

		switch (where)
		{
		case SEEK_MODE_CUR:
			dir = std::ios::cur;
			break;
		case SEEK_MODE_END:
			dir = std::ios::end;
			break;
		case SEEK_MODE_SET:
			dir = std::ios::beg;
			break;
		default:
			Int3();
			dir = std::ios::beg;
		}

		if (fp->mode & MODE_WRITE)
			fp->stream.seekg(offset, dir);

		if (fp->mode & MODE_READ)
			fp->stream.seekp(offset, dir);


		if (!fp->stream.fail())
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}

	int tell(FileHandle* fp)
	{
		Assert(fp != NULL);

		if (fp->mode & MODE_WRITE)
			return static_cast<int>(fp->stream.tellp());
		else if (fp->mode & MODE_READ)
			return static_cast<int>(fp->stream.tellg());
		else
			Int3();

		return -1;
	}

	bool eof(FileHandle* fp)
	{
		Assert(fp != NULL);
		Assert(fp->mode & MODE_READ);

		// Not very elegant but sadly needed
		if (fp->stream.peek() == EOF)
		{
			return true;
		}

		return fp->stream.eof();
	}

	int fileLength(FileHandle* handle)
	{
		int curr = tell(handle);

		seek(handle, 0, SEEK_MODE_END);

		int length = tell(handle);

		seek(handle, curr, SEEK_MODE_SET);

		return length;
	}

	template<>
	char read<char>(FileHandle* handle)
	{
		Assert(handle != NULL);

		char c;

		if (read(&c, sizeof(c), 1, handle) != 1)
			return 0;

		return c;
	}

	template<>
	ubyte read<ubyte>(FileHandle* handle)
	{
		Assert(handle != NULL);

		ubyte c;

		if (read(&c, sizeof(c), 1, handle) != 1)
			return 0;

		return c;
	}

	template<>
	int read<int>(FileHandle* handle)
	{
		Assert(handle != NULL);

		int i;

		if (read(&i, sizeof(i), 1, handle) != 1)
			return 0;

		i = INTEL_INT(i);
		return i;
	}

	template<>
	short read<short>(FileHandle* handle)
	{
		Assert(handle != NULL);

		short s;

		if (read(&s, sizeof(s), 1, handle) != 1)
			return 0;

		s = INTEL_SHORT(s);
		return s;
	}

	template<>
	ushort read<ushort>(FileHandle* handle)
	{
		Assert(handle != NULL);

		ushort s;

		if (read(&s, sizeof(s), 1, handle) != 1)
			return 0;

		s = INTEL_SHORT(s);
		return s;
	}

	template<>
	uint read<uint>(FileHandle* handle)
	{
		Assert(handle != NULL);

		uint i;

		if (read(&i, sizeof(i), 1, handle) != 1)
			return 0;

		i = INTEL_INT(i);
		return i;
	}

	template<>
	float read<float>(FileHandle* handle)
	{
		Assert(handle != NULL);

		float f;

		if (read(&f, sizeof(f), 1, handle) != 1)
			return 0.0f;

		f = INTEL_FLOAT(&f);
		return f;
	}

	template<>
	double read<double>(FileHandle* handle)
	{
		Assert(handle != NULL);

		double f;

		if (read(&f, sizeof(f), 1, handle) != 1)
			return 0.0;

		f = INTEL_FLOAT(&f);
		return f;
	}

	template<>
	vec3d read<vec3d>(FileHandle* handle)
	{
		Assert(handle != NULL);

		vec3d v;

		v.xyz.x = read<float>(handle);
		v.xyz.y = read<float>(handle);
		v.xyz.z = read<float>(handle);

		return v;
	}

	int read(void* buf, int elsize, int nelem, FileHandle* handle)
	{
		Assert(handle != NULL);
		Assert(handle->mode & MODE_READ);

		if (handle->maxReadLength > 0)
		{
			std::streampos diff = handle->stream.tellg() - handle->readLengthStart;

			Assertion(diff >= 0, "Seeked before read length restriction start!");

			if (diff > handle->maxReadLength)
			{
				std::ostringstream os;

				os << "Attempted to read " << (diff.seekpos() - handle->maxReadLength) << "-byte(s) beyond length limit";

				throw MaxReadLengthException(os.str());
			}
		}

		handle->stream.read(reinterpret_cast<char*>(buf), elsize * nelem);

		return static_cast<int>(handle->stream.gcount()) / elsize;
	}

	void readString(char* buf, int n, FileHandle* handle)
	{
		Assert(handle != NULL);

		char c;

		do {
			c = read<char>(handle);
			if (n > 0)	{
				*buf++ = c;
				n--;
			}
		} while (c != 0);
	}

	bool readLine(char* buf, int n, FileHandle* handle)
	{
		Assert(handle != NULL);
		Assert(buf != NULL);
		Assert(n > 0);

		char * t = buf;
		int i, c;

		for (i = 0; i<n - 1; i++) {
			do {
				char tmp_c;

				int ret = read(&tmp_c, 1, 1, handle);
				if (ret != 1) {
					*buf = 0;
					if (buf > t) {
						return true;
					}
					else {
						return false;
					}
				}
				c = int(tmp_c);
			} while (c == 13);
			*buf++ = char(c);
			if (c == '\n') break;
		}
		*buf++ = 0;

		return true;
	}

	void readStringLen(char* buf, int n, FileHandle* handle)
	{
		Assert(handle != NULL);

		int len;
		len = read<int>(handle);
		Assertion((len < n), "len: %i, n: %i", len, n);
		if (len)
			read(buf, len, 1, handle);

		buf[len] = 0;
	}

	template<>
	bool write<char>(boost::call_traits<char>::param_type val, FileHandle* handle)
	{
		return write(&val, sizeof(char), 1, handle) == 1;
	}

	template<>
	bool write<ubyte>(boost::call_traits<ubyte>::param_type val, FileHandle* handle)
	{
		return write(&val, sizeof(ubyte), 1, handle) == 1;
	}

	template<>
	bool write<short>(boost::call_traits<short>::param_type val, FileHandle* handle)
	{
		short s = INTEL_SHORT(val);
		return write(&s, sizeof(short), 1, handle) == 1;
	}

	template<>
	bool write<ushort>(boost::call_traits<ushort>::param_type val, FileHandle* handle)
	{
		ushort s = INTEL_SHORT(val);
		return write(&s, sizeof(ushort), 1, handle) == 1;
	}

	template<>
	bool write<int>(boost::call_traits<int>::param_type val, FileHandle* handle)
	{
		int i = INTEL_INT(val);
		return write(&i, sizeof(int), 1, handle) == 1;
	}

	template<>
	bool write<uint>(boost::call_traits<uint>::param_type val, FileHandle* handle)
	{
		uint i = INTEL_INT(val);
		return write(&i, sizeof(uint), 1, handle) == 1;
	}

	template<>
	bool write<float>(boost::call_traits<float>::param_type val, FileHandle* handle)
	{
		float i = INTEL_SHORT(val);
		return write(&i, sizeof(float), 1, handle) == 1;
	}

	template<>
	bool write<char const *>(boost::call_traits<const char*>::param_type buf, cfile::FileHandle* handle)
	{
		Assert(handle != NULL);

		if ((!buf) || (buf && !buf[0])) {
			return write<char>(0, handle);
		}
		int len = strlen(buf);
		if (!write(buf, len, 1, handle)){
			return 0;
		}
		return write<char>(0, handle);
	}

	int write(const void* buf, int elsize, int nelem, FileHandle* handle)
	{
		Assert(handle != NULL);
		Assert(handle->mode & MODE_WRITE);

		handle->stream.write(reinterpret_cast<const char*>(buf), elsize * nelem);

		// Assume everything was written
		return nelem;
	}

	void writeStringLen(const char* buf, FileHandle* handle)
	{
		int len = strlen(buf);

		write<int>(len, handle);

		if (len)
		{
			write(buf, len, 1, handle);
		}
	}

	void* returndata(FileHandle* handle)
	{
		// This relies heavily on the implementation in VFSPP!!!
		using namespace boost::iostreams;

		typedef stream_buffer<mapped_file> mapped_buffer;

		shared_ptr<mapped_buffer> mappedBuffer = dynamic_pointer_cast<mapped_buffer>(handle->buffer);
		Assert(mappedBuffer);

		return (*mappedBuffer)->data();
	}

	namespace checksum
	{
		namespace crc
		{
			// 16 and 32 bit checksum stuff ----------------------------------------------------------

			// CRC code for mission validation.  given to us by Kevin Bentley on 7/20/98.   Some sort of
			// checksumming code that he wrote a while ago.  
#define CRC32_POLYNOMIAL					0xEDB88320
			uint CRCTable[256];

#define CF_CHKSUM_SAMPLE_SIZE				512

			// update cur_chksum with the chksum of the new_data of size new_data_size
			ushort addShort(ushort seed, ubyte *buffer, int size)
			{
				ubyte *ptr = buffer;
				uint sum1, sum2;

				sum1 = sum2 = (int)(seed);

				while (size--)	{
					sum1 += *ptr++;
					if (sum1 >= 255) sum1 -= 255;
					sum2 += sum1;
				}
				sum2 %= 255;

				return (ushort)((sum1 << 8) + sum2);
			}

			// update cur_chksum with the chksum of the new_data of size new_data_size
			uint addLong(uint seed, ubyte *buffer, int size)
			{
				uint crc;
				ubyte *p;

				p = buffer;
				crc = seed;

				while (size--)
					crc = (crc >> 8) ^ CRCTable[(crc ^ *p++) & 0xff];

				return crc;
			}

			void init()
			{
				int i, j;
				uint crc;

				for (i = 0; i < 256; i++) {
					crc = i;

					for (j = 8; j > 0; j--) {
						if (crc & 1)
							crc = (crc >> 1) ^ CRC32_POLYNOMIAL;
						else
							crc >>= 1;
					}

					CRCTable[i] = crc;
				}
			}

			// single function convenient to use for both short and long checksums
			// NOTE : only one of chk_short or chk_long must be non-NULL (indicating which checksum to perform)
			int doSum(FileHandle *cfile, ushort *chk_short, uint *chk_long, int max_size)
			{
				ubyte cf_buffer[CF_CHKSUM_SAMPLE_SIZE];
				int is_long;
				int cf_len = 0;
				int cf_total;
				int read_size;

				// determine whether we're doing a short or long checksum
				is_long = 0;
				if (chk_short){
					Assert(!chk_long);
					*chk_short = 0;
				}
				else {
					Assert(chk_long);
					is_long = 1;
					*chk_long = 0;
				}

				// if max_size is -1, set it to be the size of the file
				if (max_size < 0){
					seek(cfile, 0, SEEK_MODE_SET);
					max_size = fileLength(cfile);
				}

				cf_total = 0;
				do {
					// determine how much we want to read
					if ((max_size - cf_total) >= CF_CHKSUM_SAMPLE_SIZE){
						read_size = CF_CHKSUM_SAMPLE_SIZE;
					}
					else {
						read_size = max_size - cf_total;
					}

					// read in some buffer
					cf_len = read(cf_buffer, 1, read_size, cfile);

					// total we've read so far
					cf_total += cf_len;

					// add the checksum
					if (cf_len > 0){
						// do the proper short or long checksum
						if (is_long){
							*chk_long = addLong(*chk_long, cf_buffer, cf_len);
						}
						else {
							*chk_short = addShort(*chk_short, cf_buffer, cf_len);
						}
					}
				} while ((cf_len > 0) && (cf_total < max_size));

				return 1;
			}

			// get the chksum of a pack file (VP)
			int cf_chksum_pack(const char *filename, uint *chk_long, bool full)
			{
				const int safe_size = 2097152; // 2 Meg
				const int header_offset = 32;  // skip 32bytes for header (header is currently smaller than this though)

				ubyte cf_buffer[CF_CHKSUM_SAMPLE_SIZE];
				int cf_len = 0;
				int cf_total;
				int read_size;
				int max_size;

				if (chk_long == NULL) {
					Int3();
					return 0;
				}

				std::FILE *fp = fopen(filename, "rb");

				if (fp == NULL) {
					*chk_long = 0;
					return 0;
				}

				*chk_long = 0;

				// get the max size
				fseek(fp, 0, SEEK_END);
				max_size = ftell(fp);

				// maybe do a chksum of the entire file
				if (full) {
					fseek(fp, 0, SEEK_SET);
				}
				// othewise it's only a partial check
				else {
					CLAMP(max_size, 0, safe_size);

					Assertion(max_size > header_offset,
						"max_size (%d) > header_offset in packfile %s", max_size, filename);
					max_size -= header_offset;

					fseek(fp, -(max_size), SEEK_END);
				}

				cf_total = 0;

				do {
					// determine how much we want to read
					if ((max_size - cf_total) >= CF_CHKSUM_SAMPLE_SIZE)
						read_size = CF_CHKSUM_SAMPLE_SIZE;
					else
						read_size = max_size - cf_total;

					// read in some buffer
					cf_len = fread(cf_buffer, 1, read_size, fp);

					// total we've read so far
					cf_total += cf_len;

					// add the checksum
					if (cf_len > 0)
						*chk_long = addLong((*chk_long), cf_buffer, cf_len);
				} while ((cf_len > 0) && (cf_total < max_size));

				fclose(fp);

				return 1;
			}
			// get the 2 byte checksum of the passed filename - return 0 if operation failed, 1 if succeeded
			int doShort(const char *filename, ushort *chksum, int max_size, DirType cf_type)
			{
				int ret_val;
				FileHandle *cfile = NULL;

				// zero the checksum
				*chksum = 0;

				// attempt to open the file
				cfile = open(filename, MODE_READ, OPEN_NORMAL, cf_type);
				if (cfile == NULL){
					return 0;
				}

				// call the overloaded cf_chksum function()
				ret_val = doSum(cfile, chksum, NULL, max_size);

				// close the file down
				close(cfile);
				cfile = NULL;

				// return the result
				return ret_val;
			}

			// get the 2 byte checksum of the passed file - return 0 if operation failed, 1 if succeeded
			// NOTE : preserves current file position
			int doShort(FileHandle *file, ushort *chksum, int max_size)
			{
				int ret_code;
				int start_pos;

				// Returns current position of file.
				start_pos = tell(file);
				if (start_pos == -1){
					return 0;
				}

				// move to the beginning of the file
				if (seek(file, 0, SEEK_MODE_SET)){
					return 0;
				}

				ret_code = doSum(file, chksum, NULL, max_size);
				// move back to the start position
				seek(file, start_pos, SEEK_MODE_SET);

				return ret_code;
			}

			// get the 32 bit CRC checksum of the passed filename - return 0 if operation failed, 1 if succeeded
			int doLong(const char *filename, uint *chksum, int max_size, DirType cf_type)
			{
				int ret_val;
				FileHandle *cfile = NULL;

				// zero the checksum
				*chksum = 0;

				// attempt to open the file
				cfile = open(filename, MODE_READ, OPEN_NORMAL, cf_type);
				if (cfile == NULL){
					return 0;
				}

				// call the overloaded cf_chksum function()
				ret_val = doSum(cfile, NULL, chksum, max_size);

				// close the file down
				close(cfile);
				cfile = NULL;

				// return the result
				return ret_val;
			}

			// get the 32 bit CRC checksum of the passed file - return 0 if operation failed, 1 if succeeded
			// NOTE : preserves current file position
			int doLong(FileHandle *file, uint *chksum, int max_size)
			{
				int ret_code;
				int start_pos;

				// Returns current position of file.
				start_pos = tell(file);
				if (start_pos == -1){
					return 0;
				}

				// move to the beginning of the file
				if (seek(file, 0, SEEK_MODE_SET)){
					return 0;
				}
				ret_code = doSum(file, NULL, chksum, max_size);
				// move back to the start position
				seek(file, start_pos, SEEK_MODE_SET);

				return ret_code;
			}
		}
	}

	namespace legacy
	{
		int read_compressed(void *buf, int elsize, int nelem, FileHandle *cfile)
		{
			char *out = (char *)buf;

			while (1)	{

				ubyte count;

				if (read(&count, 1, 1, cfile) != 1)	{
					break;
				}

				int run_span = count & 0x80;
				count &= (~0x80);
				count++;

				if (count > 0)	{
					if (run_span)	{
						// RLE'd data
						ubyte c;
						if (read(&c, 1, 1, cfile) != 1)	{
							break;
						}
						memset(out, c, count);
					}
					else {
						if (read(out, 1, count, cfile) != count)	{
							break;
						}
					}
					out += count;
					if (out >= (char *)buf + (elsize*nelem))	{
						break;
					}
				}
				else {
					break;
				}
			}

			return (out - (char *)buf) / elsize;
		}

		char *add_ext(const char *filename, const char *ext)
		{
			int flen, elen;
			static char path[MAX_PATH_LEN];

			flen = strlen(filename);
			elen = strlen(ext);
			Assert(flen < MAX_PATH_LEN);
			strcpy_s(path, filename);
			if ((flen < 4) || stricmp(path + flen - elen, ext)) {
				Assert(flen + elen < MAX_PATH_LEN);
				strcat_s(path, ext);
			}

			return path;
		}

	}

	namespace util
	{
		void generateVPChecksums()
		{
		}
	}
}
