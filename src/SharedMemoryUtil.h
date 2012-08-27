/**
*	@file	SharedMemoryUtil.h
*	@brief	便利な共有メモリ作成クラス
*/

#pragma once

#include <windows.h>
#include <sstream>
#include <boost\archive\text_woarchive.hpp>
#include <boost\archive\text_wiarchive.hpp>

/////////////////////////////////////////////
// CSharedMemoryT

template <bool t_bManaged>
class CSharedMemoryT
{
public:

	CSharedMemoryT() : m_hMap(NULL), m_pView(nullptr)
	{
	}

	~CSharedMemoryT()
	{
		if (t_bManaged)
			CloseHandle();
	}

	HANDLE	Handle() const { return m_hMap; }

	void	CloseHandle()
	{
		if (m_hMap) {
			if (m_pView) {
				::UnmapViewOfFile(m_pView);
				m_pView = nullptr;
			}
			::CloseHandle(m_hMap);
			m_hMap = NULL;
		}
	}

	void*	CreateSharedMemory(LPCTSTR sharedMemName, DWORD size)
	{
		ATLASSERT( m_hMap == NULL && m_pView == nullptr && sharedMemName );
		ATLASSERT( size >= 0 );
		m_hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, size, sharedMemName);
		ATLASSERT( m_hMap );
		m_pView	= ::MapViewOfFile(m_hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);;
		ATLASSERT( m_pView );
		return m_pView;
	}

	void*	OpenSharedMemory(LPCTSTR sharedMemName, bool bReadOnly)
	{
		ATLASSERT( m_hMap == NULL && m_pView == nullptr && sharedMemName );
		DWORD dwDesiredAccess = bReadOnly ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS;
		m_hMap = ::OpenFileMapping(dwDesiredAccess, FALSE, sharedMemName);
		if (m_hMap == NULL)
			return nullptr;
		ATLASSERT( m_hMap );
		m_pView	= ::MapViewOfFile(m_hMap, dwDesiredAccess, 0, 0, 0);;
		ATLASSERT( m_pView );
		return m_pView;
	}

	template <class T>
	bool	Serialize(const T& data, LPCTSTR sharedMemName = nullptr, bool bInheritHandle = false)
	{
		ATLASSERT( m_hMap == NULL );

		/* シリアライズ */
		std::wstringstream ss;
		boost::archive::text_woarchive ar(ss);
		ar << data;
		std::wstring serializedData = ss.str();

		/* 共有メモリ作成 */
		SECURITY_ATTRIBUTES	security_attributes = { sizeof(SECURITY_ATTRIBUTES) };
		security_attributes.bInheritHandle = bInheritHandle;
		DWORD	dwMapFileSize = (serializedData.size() + 1) * sizeof(WCHAR);
		m_hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, &security_attributes, PAGE_READWRITE, 0, dwMapFileSize, sharedMemName);
		ATLASSERT( m_hMap );
		if (m_hMap == NULL)
			return false;

		/* メモリマップされた領域に書き込む */
		LPTSTR sharedMemData = (LPTSTR)::MapViewOfFile(m_hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		::wcscpy_s(sharedMemData, serializedData.size() + 1, serializedData.c_str());
		::UnmapViewOfFile((LPCVOID)sharedMemData);
		return true;
	}

	template <class T>
	bool	Deserialize(T& data, LPCTSTR sharedMemName)
	{
		ATLASSERT( m_hMap == NULL );
		ATLASSERT( sharedMemName );

		/* 共有メモリを開く */
		m_hMap = ::OpenFileMapping(FILE_MAP_READ, FALSE, sharedMemName);
		ATLASSERT( m_hMap );
		if (m_hMap == NULL)
			return false;

		_desirialize(data);
		return true;
	}

	template <class T>
	bool	Deserialize(T& data, HANDLE hMap)
	{
		ATLASSERT( m_hMap == NULL );
		ATLASSERT( hMap );

		m_hMap = hMap;

		_desirialize(data);
		return true;
	}

private:
	template <class T>
	void	_desirialize(T& data)
	{
		/* メモリマップされた領域からデータを取り出す */
		LPTSTR sharedMemData = (LPTSTR)::MapViewOfFile(m_hMap, FILE_MAP_READ, 0, 0, 0);
		std::wstringstream ss;
		ss << sharedMemData;
		::UnmapViewOfFile((LPCVOID)sharedMemData);

		/* デシリアライズ */
		boost::archive::text_wiarchive ar(ss);
		ar >> data;
	}

	HANDLE	m_hMap;
	void*	m_pView;
};


typedef CSharedMemoryT<false>	CSharedMemoryHandle;
typedef CSharedMemoryT<true>	CSharedMemory;







