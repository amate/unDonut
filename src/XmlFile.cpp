// XmlFile.cpp

#include "stdafx.h"
#include "XmlFile.h"
#pragma comment(lib, "xmllite.lib")


///////////////////////////////////////////////////////////////////////////////
// CXmlFileWrite

// コンストラクタ
CXmlFileRead::CXmlFileRead(const CString &FilePath)
{
	if (FilePath.IsEmpty()) {
		throw _T("保存先のパスがありません");
	}

    if (FAILED(CreateXmlReader(IID_IXmlReader, reinterpret_cast<void**>(&m_pReader), 0))){
        throw _T("CreateXmlReader失敗");
    }
	
	// ファイルストリーム作成
    CComPtr<IStream> pStream;
	HRESULT hr = SHCreateStreamOnFile(FilePath, STGM_READ, &pStream);
    if (FAILED(hr)) {
        throw _T("SHCreateStreamOnFile失敗");
    }

    if (FAILED(m_pReader->SetInput(pStream))){
        throw _T("SetInput失敗");
    }

}

// デストラクタ
CXmlFileRead::~CXmlFileRead()
{
	//m_pReader->
}

// node == XmlNodeType_Elementのときelementを返す
// MoveToFirstAttribute()すれば属性名を返す
CString	CXmlFileRead::GetLocalName()
{
	LPCWSTR	strLocalName;
	if (FAILED(m_pReader->GetLocalName(&strLocalName, NULL))) {
		throw _T("GetLocalName失敗");
	}
	return CString(strLocalName);
}

CString	CXmlFileRead::GetValue()
{
	LPCWSTR	strValue;
	if (FAILED(m_pReader->GetValue(&strValue, NULL))) {
		throw _T("GetValue失敗");
	}
	return CString(strValue);
}

int		CXmlFileRead::GetValuei()
{
	return _wtoi(GetValue());
}

// externalElementの内側の要素をstrElementで返す
bool	CXmlFileRead::GetInternalElement(LPCWSTR externalElement, CString &strElement)
{
	XmlNodeType 	nodeType;
	while (m_pReader->Read(&nodeType) == S_OK) {
		if (nodeType == XmlNodeType_Element) {
			strElement = GetLocalName();
			return true;
		} else if (nodeType == XmlNodeType_EndElement) {
			if (GetLocalName() == externalElement) {
				return false;	// </externalElement>にきたので
			}
		}
	}
	throw _T("GetInternalElement失敗");
	return false;
}


//////////////////////////////////////////////////////////////////////
// CXmlFileRead2

void	CXmlFileRead2::MoveToFirstAttribute()
{
	if (__super::MoveToFirstAttribute() == false)
		throw _T("MoveToFirstAttribute失敗");
}

void	CXmlFileRead2::MoveToNextAttribute()
{
	if (__super::MoveToNextAttribute() == false)
		throw _T("MoveToNextAttribute失敗");
}





///////////////////////////////////////////////////////////////////////////////
// CXmlFileWrite

// コンストラクタ
CXmlFileWrite::CXmlFileWrite(const CString &FilePath)
{
	if (FilePath.IsEmpty()) {
		throw _T("保存先のパスがありません");
	}

    if (FAILED(CreateXmlWriter(IID_IXmlWriter, reinterpret_cast<void**>(&m_pWriter), 0))){
		throw _T("CreateXmlWriter失敗");
    }
   
	// ファイルストリーム作成
    CComPtr<IStream> pStream;
	HRESULT hr = SHCreateStreamOnFile(FilePath, STGM_CREATE | STGM_WRITE, &pStream);
    if (FAILED(hr)) {
        throw _T("SHCreateStreamOnFile失敗\n");
    }

    if (FAILED(m_pWriter->SetOutput(pStream))){
        throw _T("SetOutput失敗");
    }

    // インデント有効化
    if (FAILED(m_pWriter->SetProperty(XmlWriterProperty_Indent, TRUE))){
        throw _T("SetProperty失敗");
    }

    // <?xml version="1.0" encoding="UTF-8"?>
    if (FAILED(m_pWriter->WriteStartDocument(XmlStandalone_Omit))){
        throw _T("WriteStartDocument失敗");
    }
}

// デストラクタ
CXmlFileWrite::~CXmlFileWrite()
{
	// 開いているタグを閉じる
    if (FAILED(m_pWriter->WriteEndDocument())){
        throw _T("WriteEndDocument失敗");
    }

	// ファイルに書き込む
    if (FAILED(m_pWriter->Flush())){
        throw _T("Flush失敗");
    }
}




//	<Element>
void	CXmlFileWrite::WriteStartElement(LPCWSTR Element)
{
    if ( FAILED(m_pWriter->WriteStartElement(NULL, Element, NULL))){
        throw _T("WriteStartElement失敗");
    }
}

//	<LocalName>Value</LocalName>
void	CXmlFileWrite::WriteElementString(LPCWSTR LocalName, LPCWSTR Value)
{
	if (FAILED(m_pWriter->WriteElementString(NULL, LocalName, NULL, Value))){
        throw _T("WriteElementString失敗");
    }
}

//	<LocalName>Value</LocalName>
void	CXmlFileWrite::WriteElementValue(LPCWSTR LocalName, DWORD Value)
{
	CString temp;
	temp.Format(_T("%d"), Value);
	if (FAILED(m_pWriter->WriteElementString(NULL, LocalName, NULL, temp))){
        throw _T("WriteElementString失敗");
    }
}

//	<element	LocalName="Value">
void	CXmlFileWrite::WriteAttributeString(LPCWSTR LocalName, LPCWSTR Value)
{
	if (FAILED(m_pWriter->WriteAttributeString(NULL, LocalName, NULL, Value))){
        throw _T("WriteAttributeString失敗");
    }
}

void	CXmlFileWrite::WriteAttributeValue(LPCWSTR LocalName, DWORD dwValue)
{
	CString strValue;
	strValue.Format(_T("%d"), dwValue);
	WriteAttributeString(LocalName, strValue);
}

//	<element>	Text	</element>
void	CXmlFileWrite::WriteString(LPCWSTR	Text)
{
    if (FAILED(m_pWriter->WriteString(Text))){
        throw _T("WriteString失敗");
    }
}

//	</Element>
void	CXmlFileWrite::WriteFullEndElement()
{
    if (FAILED(m_pWriter->WriteFullEndElement())){
        throw _T("WriteFullEndElement失敗");
    }
}






