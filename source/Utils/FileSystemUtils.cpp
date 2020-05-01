//     _____                        ______             _            
//    / ____|                      |  ____|           (_)           
//   | (___   __ _ _   _  ___ ___  | |__   _ __   __ _ _ _ __   ___ 
//    \___ \ / _` | | | |/ __/ _ \ |  __| | '_ \ / _` | | '_ \ / _ \
//    ____) | (_| | |_| | (_|  __/ | |____| | | | (_| | | | | |  __/
//   |_____/ \__,_|\__,_|\___\___| |______|_| |_|\__, |_|_| |_|\___|
//                                                __/ |             
//                                               |___/              
// Copyright (C) 2011-2020
// Made by Marcus "Bitsauce" Vergara
// Distributed under the MIT license

#include <Sauce/Common.h>
#include <Sauce/Utils.h>
#include <sstream>
#include <fstream>

#include "MD5.h"

// TODO: This will only work on windows i think
#ifdef SAUCE_COMPILE_WINDOWS
#include <direct.h>
#else
#include <unistd.h>
#include "..\..\include\Sauce\Utils\MiscUtils.h"
#define MAX_PATH 1024
#endif

BEGIN_SAUCE_NAMESPACE

namespace util
{
	string getAbsoluteFilePath(const string& path)
	{
		if (path.substr(0, 2) == ":/")
		{
			return path.substr(2, string::npos);
		}
		else if (path.substr(0, 7) == "prefs:/")
		{
			return Game::Get()->getPrefPath() + path.substr(7, string::npos);
		}
		else if (path.substr(0, 5) == "bin:/")
		{
			return Game::Get()->getBinaryPath() + path.substr(5, string::npos);
		}
		return path;
	}

	void toAbsoluteFilePath(string& path)
	{
		path = getAbsoluteFilePath(path);
	}

	void toDirectoryPath(string& path)
	{
		if (path.back() != '/')
		{
			path += '/';
		}
	}

	string getWorkingDirectory()
	{
		char cwd[MAX_PATH];
		assert(getcwd(cwd, MAX_PATH) != nullptr);
		return cwd;
	}

	bool fileExists(string filePath)
	{
		ifstream s(getAbsoluteFilePath(filePath).c_str());
		bool open = s.is_open();
		s.close();
		return open;
	}

	std::string FileMD5(const std::string& fileName)
	{
		ifstream fileStream(fileName);
		if (fileStream)
		{
			std::string str;
			fileStream.seekg(0, std::ios::end);
			str.reserve(fileStream.tellg());
			fileStream.seekg(0, std::ios::beg);
			str.assign(
				std::istreambuf_iterator<char>(fileStream),
				std::istreambuf_iterator<char>()
			);
			return util::ByteArrayMD5(str);
		}
		return "INVALID_MD5";
	}

	//--------------------------------------------------------------
	// File system iterator
	//--------------------------------------------------------------
	FileSystemIterator::FileSystemIterator(const string& searchPath, const string& mask, const uint32 flags)
		: m_searchPath(searchPath)
		, m_searchMask(mask)
		, m_searchFlags(flags)
		, m_hFind(nullptr)
		, m_fdata()
	{
	}

	FileSystemIterator::~FileSystemIterator()
	{
		if (m_hFind)
		{
			FindClose(m_hFind);
		}
	}

	FileSystemIterator::iterator FileSystemIterator::begin()
	{
		// Fix up path
		string searchPath = m_searchPath;
		toAbsoluteFilePath(searchPath);
		toDirectoryPath(searchPath);
		searchPath += m_searchMask; // Add search mask

		// Find first file
		m_fdata = WIN32_FIND_DATA();

		if (m_hFind)
		{
			FindClose(m_hFind);
		}
		m_hFind = FindFirstFile(searchPath.c_str(), &m_fdata);
		
		const bool isEnd = m_hFind == INVALID_HANDLE_VALUE;
		if (isEnd)
		{
			return end();
		}

		return ++iterator(*this, false);
	}

	FileSystemIterator::iterator FileSystemIterator::end()
	{
		return iterator(*this, true);
	}

	FileSystemIterator::iterator& FileSystemIterator::iterator::operator++()
	{
		// Repeat until we've found a file/directory or until reached the end of the search
		m_directoryOrFile = nullptr;
		do 
		{
			if (FindNextFile(m_fsitr.m_hFind, &m_fsitr.m_fdata) != 0)
			{
				wchar_t filename[MAX_PATH];
				mbstowcs_s(0, filename, m_fsitr.m_fdata.cFileName, 1024);
				if (wcscmp(filename, L".") != 0 && wcscmp(filename, L"..") != 0)
				{
					if ((m_fsitr.m_fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 &&
						m_fsitr.m_searchFlags & FileSystemIteratorFlag::IncludeDirectories)
					{
						m_directoryOrFile = new DirectoryOrFile(m_fsitr.m_fdata.cFileName, m_fsitr.m_searchPath, true);
					}
					else if (m_fsitr.m_searchFlags & FileSystemIteratorFlag::IncludeFiles)
					{
						m_directoryOrFile = new DirectoryOrFile(m_fsitr.m_fdata.cFileName, m_fsitr.m_searchPath, false);
					}
				}
			}
			else
			{
				m_isEnd = true;
			}
		} while (!m_directoryOrFile && !m_isEnd);

		return *this;
	}

	FileSystemIterator::iterator FileSystemIterator::iterator::operator++(int)
	{
		iterator retval(*this);
		this->operator++();
		return retval;
	}

	DirectoryOrFile FileSystemIterator::iterator::operator*() const
	{
		return *m_directoryOrFile;
	}
}

END_SAUCE_NAMESPACE
