/**
 *	@file	MainFrameFileDropTarget.h
 */

#pragma once

#include "MtlDragDrop.h"
#include "MtlFile.h"



template <class T>
class CMainFrameFileDropTarget : public IDropTargetImpl<T> {
	bool	m_bDragAccept;

public:
	DROPEFFECT	OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point);
	DROPEFFECT	OnDragOver(IDataObject *pDataObject, DWORD dwKeyState, CPoint point, DROPEFFECT dropOkEffect);
	DROPEFFECT	OnDrop(IDataObject *pDataObject, DROPEFFECT dropEffect, DROPEFFECT dropEffectList, CPoint point);
};



template <class T>
DROPEFFECT CMainFrameFileDropTarget<T>::OnDragEnter(IDataObject *pDataObject, DWORD dwKeyState, CPoint point)
{
	m_bDragAccept = _MtlIsHlinkDataObject(pDataObject);
	return _MtlStandardDropEffect(dwKeyState);
}



template <class T>
DROPEFFECT CMainFrameFileDropTarget<T>::OnDragOver(
		IDataObject *	pDataObject,
		DWORD			dwKeyState,
		CPoint			point,
		DROPEFFECT		dropOkEffect)
{
	if (!m_bDragAccept)
		return DROPEFFECT_NONE;

	return _MtlStandardDropEffect(dwKeyState) | _MtlFollowDropEffect(dropOkEffect) | DROPEFFECT_COPY;
}



template <class T>
DROPEFFECT CMainFrameFileDropTarget<T>::OnDrop(
		IDataObject *	pDataObject,
		DROPEFFECT		dropEffect,
		DROPEFFECT		dropEffectList, CPoint point)
{
	CSimpleArray<CString> arrFiles;
	T * 				  pT = static_cast<T *>(this);

	if ( MtlGetDropFileName(pDataObject, arrFiles) ) {
		if (arrFiles.GetSize() == 1) {
			DonutOpenFile( pT->m_hWnd, arrFiles[0], DonutGetStdOpenFlag() );
			return DROPEFFECT_COPY;
		}

		for (int i = 0; i < arrFiles.GetSize(); ++i) {
			DonutOpenFile(pT->m_hWnd, arrFiles[i]);
		}

		dropEffect = DROPEFFECT_COPY;
		return true;
	}

	CString 	strText;

	if ( MtlGetHGlobalText(pDataObject, strText)
	   || MtlGetHGlobalText( pDataObject, strText, ::RegisterClipboardFormat(CFSTR_SHELLURL) ) )
	{
		DonutOpenFile( pT->m_hWnd, strText, DonutGetStdOpenFlag() );
		return DROPEFFECT_COPY;
	}

	return DROPEFFECT_NONE;
}
