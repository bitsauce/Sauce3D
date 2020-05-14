// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#pragma once

#include <Sauce/Config.h>

#ifdef SAUCE_COMPILE_WINDOWS
#include <Windows.h>
#endif

BEGIN_SAUCE_NAMESPACE

namespace util
{
	//--------------------------------------------------------------
	// File path functions
	//--------------------------------------------------------------
	SAUCE_API bool fileExists(string filePath);
	SAUCE_API string getAbsoluteFilePath(const string& assetPath);
	SAUCE_API void toAbsoluteFilePath(string& assetPath);
	SAUCE_API void toDirectoryPath(string& path);
	SAUCE_API string getWorkingDirectory();

	//--------------------------------------------------------------
	// File hashing
	//--------------------------------------------------------------
	SAUCE_API std::string FileMD5(const std::string& fileName);

	/**
	 * \enum	FileSystemIteratorFlag
	 *
	 * \brief	Flags controlling how the FileSystemIterator will iterate files
	 */
	enum class FileSystemIteratorFlag : uint32
	{
		IncludeDirectories = 1 << 1, ///< Include directories if set
		IncludeFiles       = 1 << 2  ///< Include files if set
	};
	ENUM_CLASS_ADD_BITWISE_OPERATORS(FileSystemIteratorFlag);

	//--------------------------------------------------------------
	// File system iterator
	//--------------------------------------------------------------
	template<long FROM, long TO>
	class Range {
	public:
		class iterator {
			long num = FROM;
		public:
			iterator(long _num = 0) : num(_num) {}
			iterator& operator++() { num = TO >= FROM ? num + 1 : num - 1; return *this; }
			iterator operator++(int) { iterator retval = *this; ++(*this); return retval; }
			bool operator==(iterator other) const { return num == other.num; }
			bool operator!=(iterator other) const { return !(*this == other); }
			long operator*() { return num; }
			// iterator traits
			using difference_type = long;
			using value_type = long;
			using pointer = const long*;
			using reference = const long&;
			using iterator_category = std::forward_iterator_tag;
		};
		iterator begin() { return FROM; }
		iterator end() { return TO >= FROM ? TO + 1 : TO - 1; }
	};

	struct SAUCE_API DirectoryOrFile
	{
		DirectoryOrFile(const string& baseName, const string& directoryName, const bool& isDirectory)
			: baseName(baseName)
			, directoryName(directoryName)
			, fullPath(directoryName + "/" + baseName)
			, isDirectory(false)
		{
		}

		bool operator==(const DirectoryOrFile& other) const
		{
			return fullPath == other.fullPath && isDirectory == other.isDirectory;
		}

		const string baseName;
		const string directoryName;
		const string fullPath;
		const bool isDirectory;
	};

	class SAUCE_API FileSystemIterator
	{
	public:
		class iterator
		{
			friend class FileSystemIterator;

			FileSystemIterator& m_fsitr;
			DirectoryOrFile* m_directoryOrFile;
			bool m_isEnd;

			iterator(FileSystemIterator& fsitr, bool isEnd)
				: m_fsitr(fsitr)
				, m_directoryOrFile(nullptr)
				, m_isEnd(isEnd)
			{
			}

			iterator(const iterator& other)
				: m_fsitr(other.m_fsitr)
				, m_directoryOrFile(nullptr)
				, m_isEnd(other.m_isEnd)
			{
				if (other.m_directoryOrFile)
				{
					m_directoryOrFile = new DirectoryOrFile(*other.m_directoryOrFile);
				}
			}

		public:
			~iterator()
			{
				delete m_directoryOrFile;
			}

			iterator& operator++();
			iterator operator++(int);
			bool operator==(const iterator& other) const
			{
				if (m_directoryOrFile == nullptr || other.m_directoryOrFile == nullptr)
				{
					return m_isEnd == other.m_isEnd && m_directoryOrFile == other.m_directoryOrFile;
				}
				else
				{
					return m_isEnd == other.m_isEnd && *m_directoryOrFile == *other.m_directoryOrFile;
				}
			}
			bool operator!=(const iterator& other) const { return !this->operator==(other); }
			DirectoryOrFile operator*() const;

			// iterator traits
			using difference_type = DirectoryOrFile;
			using value_type = DirectoryOrFile;
			using pointer = const DirectoryOrFile*;
			using reference = const DirectoryOrFile&;
			using iterator_category = std::forward_iterator_tag;
		};

		iterator begin();
		iterator end();

		FileSystemIterator(const string& searchPath, const string& mask, const uint32 flags);
		~FileSystemIterator();

	private:
		const string m_searchPath;
		const string m_searchMask;
		const uint32 m_searchFlags;

		WIN32_FIND_DATA m_fdata;
		HANDLE m_hFind;
	};
}

class SAUCE_API ByteStreamOut
{
	ofstream fileStream;

public:
	ByteStreamOut(const string& filePath)
		: fileStream(filePath, ofstream::binary)
	{
	}

	ByteStreamOut(const ByteStreamOut&) = delete;

	operator bool() const { return (bool)fileStream; }
	void close() { fileStream.close(); }

	friend ByteStreamOut& operator<<(ByteStreamOut& out, const string& v)
	{
		const size_t strSize = v.size();
		out.fileStream.write((const char*)&strSize, sizeof(size_t));
		out.fileStream.write((const char*)v.c_str(), strSize);
		return out;
	}

	template<typename T>
	friend ByteStreamOut& operator<<(ByteStreamOut& out, const vector<T>& v)
	{
		const size_t arraySize = v.size();
		out.fileStream.write((const char*)&arraySize, sizeof(size_t));
		for (int32 i = 0; i < arraySize; ++i)
		{
			out << v[i];
		}
		return out;
	}

	template<typename K, typename V>
	friend ByteStreamOut& operator<<(ByteStreamOut& out, const unordered_map<K, V>& v)
	{
		const size_t mapSize = v.size();
		out.fileStream.write((const char*)&mapSize, sizeof(size_t));
		for (const pair<K, V>& entry : v)
		{
			out << entry.first;
			out << entry.second;
		}
		return out;
	}

	friend ByteStreamOut& operator<<(ByteStreamOut& out, const bool& v)
	{
		out.fileStream.write((const char*)&v, sizeof(bool));
		return out;
	}

	friend ByteStreamOut& operator<<(ByteStreamOut& out, const float& v)
	{
		out.fileStream.write((const char*)&v, sizeof(float));
		return out;
	}

	friend ByteStreamOut& operator<<(ByteStreamOut& out, const double& v)
	{
		out.fileStream.write((const char*)&v, sizeof(double));
		return out;
	}

	friend ByteStreamOut& operator<<(ByteStreamOut& out, const int8& v)
	{
		out.fileStream.write((const char*)&v, sizeof(int8));
		return out;
	}

	friend ByteStreamOut& operator<<(ByteStreamOut& out, const uint8& v)
	{
		out.fileStream.write((const char*)&v, sizeof(uint8));
		return out;
	}

	friend ByteStreamOut& operator<<(ByteStreamOut& out, const int16& v)
	{
		out.fileStream.write((const char*)&v, sizeof(int16));
		return out;
	}

	friend ByteStreamOut& operator<<(ByteStreamOut& out, const uint16& v)
	{
		out.fileStream.write((const char*)&v, sizeof(uint16));
		return out;
	}

	friend ByteStreamOut& operator<<(ByteStreamOut& out, const int32& v)
	{
		out.fileStream.write((const char*)&v, sizeof(int32));
		return out;
	}

	friend ByteStreamOut& operator<<(ByteStreamOut& out, const uint32& v)
	{
		out.fileStream.write((const char*)&v, sizeof(uint32));
		return out;
	}

	friend ByteStreamOut& operator<<(ByteStreamOut& out, const int64& v)
	{
		out.fileStream.write((const char*)&v, sizeof(int64));
		return out;
	}

	friend ByteStreamOut& operator<<(ByteStreamOut& out, const uint64& v)
	{
		out.fileStream.write((const char*)&v, sizeof(uint64));
		return out;
	}
};

class SAUCE_API ByteStreamIn
{
	ifstream fileStream;

public:
	ByteStreamIn(const string& filePath)
		: fileStream(filePath, ifstream::binary)
	{
	}

	ByteStreamIn(const ByteStreamIn&) = delete;

	operator bool() const { return (bool)fileStream; }
	void close() { fileStream.close(); }

	friend ByteStreamIn& operator>>(ByteStreamIn& in, string& v)
	{
		size_t strSize;
		in.fileStream.read((char*)&strSize, sizeof(size_t));
		v.resize(strSize);
		in.fileStream.read((char*)v.c_str(), strSize);
		return in;
	}

	template<typename T>
	friend ByteStreamIn& operator>>(ByteStreamIn& in, vector<T>& v)
	{
		size_t arraySize = 0;
		in.fileStream.read((char*)&arraySize, sizeof(size_t));
		v.resize(arraySize);
		for (int32 i = 0; i < arraySize; ++i)
		{
			in >> v[i];
		}
		return in;
	}

	template<typename K, typename V>
	friend ByteStreamIn& operator>>(ByteStreamIn& in, unordered_map<K, V>& v)
	{
		size_t mapSize = 0;
		in.fileStream.read((char*)&mapSize, sizeof(size_t));
		for (uint32 i = 0; i < mapSize; ++i)
		{
			pair<K, V> entry;
			in >> entry.first;
			in >> entry.second;
			v.insert(entry);
		}
		return in;
	}

	friend ByteStreamIn& operator>>(ByteStreamIn& in, bool& v)
	{
		in.fileStream.read((char*)&v, sizeof(bool));
		return in;
	}

	friend ByteStreamIn& operator>>(ByteStreamIn& in, float& v)
	{
		in.fileStream.read((char*)&v, sizeof(float));
		return in;
	}

	friend ByteStreamIn& operator>>(ByteStreamIn& in, double& v)
	{
		in.fileStream.read((char*)&v, sizeof(double));
		return in;
	}

	friend ByteStreamIn& operator>>(ByteStreamIn& in, int8& v)
	{
		in.fileStream.read((char*)&v, sizeof(int8));
		return in;
	}

	friend ByteStreamIn& operator>>(ByteStreamIn& in, uint8& v)
	{
		in.fileStream.read((char*)&v, sizeof(uint8));
		return in;
	}

	friend ByteStreamIn& operator>>(ByteStreamIn& in, int16& v)
	{
		in.fileStream.read((char*)&v, sizeof(int16));
		return in;
	}

	friend ByteStreamIn& operator>>(ByteStreamIn& in, uint16& v)
	{
		in.fileStream.read((char*)&v, sizeof(uint16));
		return in;
	}

	friend ByteStreamIn& operator>>(ByteStreamIn& in, int32& v)
	{
		in.fileStream.read((char*)&v, sizeof(int32));
		return in;
	}

	friend ByteStreamIn& operator>>(ByteStreamIn& in, uint32& v)
	{
		in.fileStream.read((char*)&v, sizeof(uint32));
		return in;
	}

	friend ByteStreamIn& operator>>(ByteStreamIn& in, int64& v)
	{
		in.fileStream.read((char*)&v, sizeof(int64));
		return in;
	}

	friend ByteStreamIn& operator>>(ByteStreamIn& in, uint64& v)
	{
		in.fileStream.read((char*)&v, sizeof(uint64));
		return in;
	}
};

END_SAUCE_NAMESPACE
