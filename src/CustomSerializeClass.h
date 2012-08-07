/**
*	@file	CustomSerializeClass.h
*	@brief	ユーザー定義クラスをシリアライズするのに必要
*/

#pragma once


namespace boost {
  namespace serialization {

	template <class Archive>
	void save(Archive& ar, const WTL::CString& s, const unsigned int /*version*/)
	{
		const std::wstring ss(s);
		ar & ss;
	}

	template<class Archive>
	void load(Archive& ar, WTL::CString& s, const unsigned int /*version*/) 
	{
		std::wstring ss;
		ar & ss;
		s = ss.c_str();
	}


    template <class Archive>
	void serialize(Archive& ar, WTL::CString& s, const unsigned int version)
	{
		boost::serialization::split_free(ar, s, version);
	}
  }
}


