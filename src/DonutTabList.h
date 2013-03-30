/**
*	@file	DonutTabList.h
*	@brief	タブリスト管理クラス
*/

#pragma once

#include <vector>
#include <memory>
#include "ChildFrame.h"

class CDonutTabList
{
public:
	CDonutTabList();

	CDonutTabList(CDonutTabList&& tabList);

	/// filePath からタブリストを読み込む
	bool	Load(const CString& filePath);

	/// filePath にタブリストを保存する
	bool	Save(const CString& filePath, bool bBackup = true);

	// Attributes
	int		GetActiveIndex() const { return m_nActiveIndex; }
	void	SetActiveIndex(int nActiveIndex) { m_nActiveIndex = nActiveIndex; }
	int		GetCount() const { return static_cast<int>(m_vecpChildFrameData.size()); }
	std::unique_ptr<ChildFrameDataOnClose>&	At(int nIndex) { return m_vecpChildFrameData.at(nIndex); }

	std::vector<std::unique_ptr<ChildFrameDataOnClose>>::iterator	begin() { return m_vecpChildFrameData.begin(); }
	std::vector<std::unique_ptr<ChildFrameDataOnClose>>::iterator	end() { return m_vecpChildFrameData.end(); }

	// Operates
	void	Delete(int nIndex) { m_vecpChildFrameData.erase(m_vecpChildFrameData.begin() + nIndex); }
	void	PushBack(std::unique_ptr<ChildFrameDataOnClose>&& pData) { m_vecpChildFrameData.push_back(std::move(pData)); }
	void	Swap(std::vector<std::unique_ptr<ChildFrameDataOnClose>>& vecData) { m_vecpChildFrameData.swap(vecData); }

private:
	// Data members
	std::vector<std::unique_ptr<ChildFrameDataOnClose>>	m_vecpChildFrameData;
	int	m_nActiveIndex;
};

