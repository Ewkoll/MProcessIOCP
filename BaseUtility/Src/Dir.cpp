#include "Dir.h"
#include <boost/filesystem.hpp>

std::list<std::string> CDir::EnumFilePath(const std::string& strPath)
{
	boost::filesystem::path DirData(strPath);
	std::list<std::string> ListStrResult;
	
	if (boost::filesystem::exists(DirData))
	{
		boost::filesystem::directory_iterator it(DirData);
		boost::filesystem::directory_iterator itEnd;

		for (; it != itEnd; ++it)
		{
			bool bDir = boost::filesystem::is_directory(it->path());
			if (!bDir)
			{
				ListStrResult.push_back(it->path().string());
			}
		}
	}
	return ListStrResult;
}

void CDir::EnumFilePath(const std::string& strPath, CDirVistor& Vistor)
{
	boost::filesystem::path DirData(strPath);

	if (boost::filesystem::exists(DirData))
	{
		boost::filesystem::directory_iterator it(DirData);
		boost::filesystem::directory_iterator itEnd;

		for (; it != itEnd; ++it)
		{
			bool bDir = boost::filesystem::is_directory(it->path());

			std::string strFile = it->path().string();
			std::string strPath = it->path().parent_path().string();
			std::string strExtension = it->path().extension().string();
			Vistor.Vistor(bDir, strFile, strPath, strExtension);
		}
	}
}

void CDir::EnumFilePath(bool bRecursive, const std::string& strPath, CDirVistor& Vistor)
{
	boost::filesystem::path DirData(strPath);

	if (boost::filesystem::exists(DirData))
	{
		boost::filesystem::recursive_directory_iterator it(DirData);
		boost::filesystem::recursive_directory_iterator itEnd;

		for (; it != itEnd; ++it)
		{
			bool bDir = boost::filesystem::is_directory(it->path());

			std::string strFile = it->path().string();
			std::string strPath = it->path().parent_path().string();
			std::string strExtension = it->path().extension().string();
			Vistor.Vistor(bDir, strFile, strPath, strExtension);
			if (bDir && bRecursive)
			{
				EnumFilePath(strPath, Vistor);
			}
		}
	}
}