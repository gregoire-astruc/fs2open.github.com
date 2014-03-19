/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/ 



#ifndef __CFILE_H__
#define __CFILE_H__

#include "globalincs/pstypes.h"

#include <boost/call_traits.hpp>

namespace cfile
{
	struct FileHandle;

	// exceptions and other errors
	class Exception : public std::exception
	{
	public:
		Exception() : m_excuse("CFILE Exception")
		{
		}

		Exception(const std::string &excuse) : m_excuse(excuse)
		{
		}

		~Exception() throw()
		{
		}

		virtual const char *what() const throw()
		{
			return m_excuse.c_str();
		}

	private:
		std::string m_excuse;
	};

	class MaxReadLengthException : public Exception
	{
	public:
		MaxReadLengthException(const std::string &excuse) : Exception(excuse)
		{
		}

		MaxReadLengthException() : Exception("Attempted to read beyond length limit")
		{
		}
	};

	class EOFException : public Exception
	{
	public:
		EOFException(const std::string &excuse) : Exception(excuse)
		{
		}

		EOFException() : Exception("End of file has been reached.")
		{
		}
	};

	enum OpenType
	{
		OPEN_NORMAL = 0,			// open file normally
		OPEN_MEMORY_MAPPED = (1<<0)	//	open file as a memory-mapped file
	};

	enum OpenMode
	{
		MODE_READ = 1 << 0,
		MODE_WRITE = 1 << 1
	};

	enum DirType
	{		
		TYPE_INVALID			= 0,

		TYPE_ROOT				= 1,			// Root must be 1!!
		TYPE_DATA				= 2,
		TYPE_MAPS				= 3,
		TYPE_TEXT				= 4,
		TYPE_MODELS				= 5,
		TYPE_TABLES				= 6,
		TYPE_SOUNDS				= 7,
		TYPE_SOUNDS_8B22K		= 8,
		TYPE_SOUNDS_16B11K		= 9,
		TYPE_VOICE				= 10,
		TYPE_VOICE_BRIEFINGS	= 11,
		TYPE_VOICE_CMD_BRIEF	= 12,
		TYPE_VOICE_DEBRIEFINGS	= 13,
		TYPE_VOICE_PERSONAS		= 14,
		TYPE_VOICE_SPECIAL		= 15,
		TYPE_VOICE_TRAINING		= 16,
		TYPE_MUSIC				= 17,
		TYPE_MOVIES				= 18,
		TYPE_INTERFACE			= 19,
		TYPE_FONT				= 20,
		TYPE_EFFECTS			= 21,
		TYPE_HUD				= 22,
		TYPE_PLAYERS			= 23,
		TYPE_PLAYER_IMAGES		= 24,
		TYPE_SQUAD_IMAGES		= 25,
		TYPE_SINGLE_PLAYERS		= 26,
		TYPE_SINGLE_PLAYERS_INFERNO = 27,
		TYPE_MULTI_PLAYERS		= 28,
		TYPE_MULTI_PLAYERS_INFERNO = 29,
		TYPE_CACHE				= 30,
		TYPE_MULTI_CACHE		= 31,
		TYPE_MISSIONS			= 32,
		TYPE_CONFIG				= 33,
		TYPE_DEMOS				= 34,
		TYPE_CBANIMS			= 35,
		TYPE_INTEL_ANIMS		= 36,
		TYPE_SCRIPTS			= 37,
		TYPE_FICTION			= 38,

		MAX_PATH_TYPES,

		TYPE_ANY				= -1,		// Used to check in any directory
	};

	enum SeekMode
	{
		SEEK_MODE_SET = 0,
		SEEK_MODE_CUR = 1,
		SEEK_MODE_END = 2,
	};

	enum SortMode
	{
		SORT_NONE = 0,
		SORT_NAME = 1,
		SORT_TIME = 2,
		SORT_REVERSE = 3,
	};

	typedef bool(*ListFilterFunction)(const std::string&);

	bool init(const std::wstring& cdromTime = L"");

	void shutdown();

	SCP_string getRootDir();

	void listFiles(SCP_vector<SCP_string>& names, DirType pathType, const SCP_string& filter = "", SortMode sortMode = SORT_NONE, ListFilterFunction = NULL, bool returnFullPath = false);
	
	void listFiles(SCP_vector<SCP_string>& names, const SCP_string& pathType, const SCP_string& filter = "", SortMode sortMode = SORT_NONE, ListFilterFunction = NULL, bool returnFullPath = false);

	bool exists(const SCP_string& path, DirType type = TYPE_ANY, bool localize = false);

	bool findFile(const SCP_string& name, SCP_string& outName, DirType type = TYPE_ANY, const char** exts = NULL, size_t numExts = 0, size_t* out_extIndex = NULL, bool localize = false);

	bool access(const SCP_string& name, DirType type, int mode);

	bool deleteFile(const SCP_string& path, DirType type = TYPE_ANY, bool localize = false);

	size_t flushDir(DirType type);

	DirType getPathType(const SCP_string& path);

	bool rename(const SCP_string& oldName, const SCP_string& newName, DirType type = TYPE_ANY, bool localize = false);

	FileHandle* open(const SCP_string& file_path, int mode = MODE_READ, OpenType type = OPEN_NORMAL, DirType dir_type = TYPE_ANY, bool localize = false);

	bool close(FileHandle* handle);

	std::iostream& getStream(FileHandle* handle);

	void setMaxReadLength(FileHandle* handle, size_t size);

	const std::string& getFilePath(FileHandle* handle);

	bool flush(FileHandle* handle);

	int seek(FileHandle *fp, int offset, cfile::SeekMode where);

	int tell(FileHandle* fp);

	bool eof(FileHandle* fp);

	int fileLength(FileHandle* handle);

	template<class T>
	T read(FileHandle* handle);

	int read(void* buf, int elsize, int nelem, FileHandle* handle);

	void readString(char* buf, int n, FileHandle* handle);

	bool readLine(char* buf, int n, FileHandle* handle);

	/**
	* @brief Read a fixed length string that is not null-terminated, with the length stored in file
	*
	* @param buf Pre-allocated array to store string
	* @param n Size of pre-allocated array
	* @param file File to read from
	*
	* @note Appends NULL character to string (buf)
	*/
	void readStringLen(char* buf, int n, FileHandle* handle);

	template<class T>
	bool write(typename boost::call_traits<T>::param_type val, FileHandle* handle);

	int write(const void* buf, int elsize, int nelem, FileHandle* handle);

	void writeStringLen(const char* buf, FileHandle* handle);

	void* returndata(FileHandle* handle);

	namespace checksum
	{
		namespace crc
		{
			// get the 2 byte checksum of the passed filename - return 0 if operation failed, 1 if succeeded
			int doShort(const char *filename, ushort *chksum, int max_size = -1, DirType cf_type = TYPE_ANY);

			// get the 2 byte checksum of the passed file - return 0 if operation failed, 1 if succeeded
			// NOTE : preserves current file position
			int doShort(cfile::FileHandle *file, ushort *chksum, int max_size = -1);

			// get the 32 bit CRC checksum of the passed filename - return 0 if operation failed, 1 if succeeded
			int doLong(const char *filename, uint *chksum, int max_size = -1, DirType cf_type = TYPE_ANY);

			// get the 32 bit CRC checksum of the passed file - return 0 if operation failed, 1 if succeeded
			// NOTE : preserves current file position
			int doLong(cfile::FileHandle *file, uint *chksum, int max_size = -1);

			int pack(const char *filename, uint *chk_long, bool full = false);

			// convenient for misc checksumming purposes ------------------------------------------

			// update cur_chksum with the chksum of the new_data of size new_data_size
			ushort doShort(ushort seed, ubyte *buffer, int size);

			// update cur_chksum with the chksum of the new_data of size new_data_size
			uint doLong(uint seed, ubyte *buffer, int size);

			// convenient for misc checksumming purposes ------------------------------------------

			// update cur_chksum with the chksum of the new_data of size new_data_size
			ushort addShort(ushort seed, ubyte *buffer, int size);

			// update cur_chksum with the chksum of the new_data of size new_data_size
			uint addLong(uint seed, ubyte *buffer, int size);

			void init();
		}
	}

	namespace legacy
	{
		int read_compressed(void *buf, int elsize, int nelem, FileHandle *cfile);

		char *add_ext(const char *filename, const char *ext);
	}

	namespace util
	{
		void generateVPChecksums();

		template<class String>
		void removeExtension(String& string)
		{
			String::size_type dot = string.find_last_of(".");

			if (dot != String::npos)
			{
				string.resize(dot);
			}
		}

#ifdef WIN32
		const char PlatformDirectorySeparator = '\\';
#else
		const char PlatformDirectorySeparator = '/';
#endif
	}
}


#endif	/* __CFILE_H__ */
