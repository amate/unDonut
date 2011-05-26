// XmlFile.h
#pragma once

#include <XmlLite.h>

////////////////////////////////////////////////////////////////////////////
// CXmlFileRead

class CXmlFileRead
{
private:
	CComPtr<IXmlReader>	m_pReader;

public:
	// コンストラクタ/デストラクタ
	CXmlFileRead(const CString &FilePath);
	virtual ~CXmlFileRead();

	HRESULT	Read(XmlNodeType* pNodeType) { return m_pReader->Read(pNodeType); }

	CString	GetLocalName();
	CString	GetValue();
	int		GetValuei();

	bool	GetInternalElement(LPCWSTR externalElement, CString &strElement);

	bool	MoveToFirstAttribute() { return m_pReader->MoveToFirstAttribute() == S_OK; }
	bool	MoveToNextAttribute() { return m_pReader->MoveToNextAttribute() == S_OK; }
};

///////////////////////////////////////////////////////////////////////////
// CXmlFileRead2

class CXmlFileRead2 : public CXmlFileRead
{
public:
	CXmlFileRead2(const CString& filePath) 
		: CXmlFileRead(filePath)
	{	}

	bool	Read(XmlNodeType* pNodeType) { return __super::Read(pNodeType) == S_OK; }

	void	MoveToFirstAttribute();
	void	MoveToNextAttribute();
};

////////////////////////////////////////////////////////////////////////////
// CXmlFileWrite

class CXmlFileWrite
{
private:
	CComPtr<IXmlWriter> m_pWriter;

public:
	// コンストラクタ/デストラクタ
	CXmlFileWrite(const CString &FilePath);
	~CXmlFileWrite();

	void	WriteStartElement(LPCWSTR Element);

	void	WriteElementString(LPCWSTR LocalName, LPCWSTR Value);
	void	WriteElementValue(LPCWSTR LocalName, DWORD Value);
	void	WriteAttributeString(LPCWSTR LocalName, LPCWSTR Value);
	void	WriteAttributeValue(LPCWSTR LocalName, DWORD dwValue);
	void	WriteString(LPCWSTR	Text);

	void	WriteFullEndElement();
};


















