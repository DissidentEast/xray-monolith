#pragma once
/*
#include "shared_data.h"
#include "PhraseScript.h"

#include "xml_str_id_loader.h"
#include "encyclopedia_article_defs.h"

#include "PhraseDialogDefs.h"


struct SInfoPortionData : CSharedResource
{
						SInfoPortionData ();
	virtual				~SInfoPortionData ();

	//Ð¼Ð°ÑÑÐ¸Ð² Ñ Ð¸Ð¼ÐµÐ½Ð°Ð¼Ð¸ Ð´Ð¸Ð°Ð»Ð¾Ð³Ð¾Ð², ÐºÐ¾Ñ‚Ð¾Ñ€Ñ‹Ðµ Ð¼Ð¾Ð³ÑƒÑ‚ Ð±Ñ‹Ñ‚ÑŒ Ð¸Ð½Ð¸Ñ†Ð¸Ð¸Ñ€Ð¾Ð²Ð°Ð½Ñ‹
	//Ð¸Ð· ÑÑ‚Ð¾Ð³Ð¾ InfoPortion
	DIALOG_ID_VECTOR	m_DialogNames;

	//ÑÐ¿Ð¸ÑÐ¾Ðº ÑÑ‚Ð°Ñ‚ÐµÐ¹ Ð² ÑÐ½Ñ†Ð¸ÐºÐ»Ð¾Ð¿ÐµÐ´Ð¸Ð¸, ÐºÐ¾Ñ‚Ð¾Ñ€Ñ‹Ðµ ÑÑ‚Ð°Ð½Ð¾Ð²ÑÑ‚ÑÑ Ð¸Ð·Ð²ÐµÑÑ‚Ð½Ñ‹Ð¼Ð¸ 
	ARTICLE_ID_VECTOR	m_Articles;
	//ÑÐ¿Ð¸ÑÐ¾Ðº ÑÑ‚Ð°Ñ‚ÐµÐ¹ Ð² ÑÐ½Ñ†Ð¸ÐºÐ»Ð¾Ð¿ÐµÐ´Ð¸Ð¸, ÐºÐ¾Ñ‚Ð¾Ñ€Ñ‹Ðµ ÑÑ‚Ð°Ð½Ð¾Ð²ÑÑ‚ÑÑ Ð½ÐµÐ¸Ð·Ð²ÐµÑÑ‚Ð½Ñ‹Ð¼Ð¸ (Ð½Ð° Ñ‚Ð¾Ñ‚ ÑÐ»ÑƒÑ‡Ð°Ð¹ ÐµÑÐ»Ð¸
	//Ð½ÑƒÐ¶Ð½Ð¾ Ð·Ð°Ð¼ÐµÐ½Ð¸Ñ‚ÑŒ Ð¾Ð´Ð½Ñƒ ÑÑ‚Ð°Ñ‚ÑŒÑŽ Ð´Ñ€ÑƒÐ³Ð¾Ð¹)
	ARTICLE_ID_VECTOR	m_ArticlesDisable;
    	
	//ÑÐºÑ€Ð¸Ð¿Ñ‚Ð¾Ð²Ñ‹Ðµ Ð´ÐµÐ¹ÑÑ‚Ð²Ð¸Ñ, ÐºÐ¾Ñ‚Ð¾Ñ€Ñ‹Ðµ Ð°ÐºÑ‚Ð¸Ð²Ð¸Ñ€ÑƒÐµÑ‚ÑÑ Ð¿Ð¾ÑÐ»Ðµ Ñ‚Ð¾Ð³Ð¾ ÐºÐ°Ðº 
	//Ð¸Ð½Ñ„Ð¾Ñ€Ð¼Ð°Ñ†Ð¸ÑŽ Ð¿Ð¾Ð»ÑƒÑ‡Ð°ÐµÑ‚ Ð¿ÐµÑ€ÑÐ¾Ð½Ð°Ð¶
	CDialogScriptHelper		m_InfoScriptHelper;

	//Ð¼Ð°ÑÑÐ¸Ð² Ñ Ð¸Ð½Ð´ÐµÐºÑÐ°Ð¼Ð¸ Ñ‚ÐµÑ… Ð¿Ð¾Ñ€Ñ†Ð¸Ð¹ Ð¸Ð½Ñ„Ð¾Ñ€Ð¼Ð°Ñ†Ð¸Ð¸, ÐºÐ¾Ñ‚Ð¾Ñ€Ñ‹Ðµ
	//Ð¸ÑÑ‡ÐµÐ·Ð½ÑƒÑ‚, Ð¿Ð¾ÑÐ»Ðµ Ð¿Ð¾Ð»ÑƒÑ‡ÐµÐ½Ð¸Ñ ÑÑ‚Ð¾Ð¹ info_portion
	DEFINE_VECTOR		(shared_str, INFO_ID_VECTOR, INFO_ID_VECTOR_IT);
	INFO_ID_VECTOR		m_DisableInfo;
};


class CInfoPortion;

//ÐºÐ²Ð°Ð½Ñ‚  - Ð¿Ð¾Ñ€Ñ†Ð¸Ñ Ð¸Ð½Ñ„Ð¾Ñ€Ð¼Ð°Ñ†Ð¸Ð¸
class CInfoPortion : public CSharedClass<SInfoPortionData, shared_str, false>,
					 public CXML_IdToIndex<CInfoPortion>
{
private:
	typedef CSharedClass<SInfoPortionData, shared_str, false>	inherited_shared;
	typedef CXML_IdToIndex<CInfoPortion>						id_to_index;

	friend id_to_index;
public:
				CInfoPortion	(void);
	virtual		~CInfoPortion	(void);

	//Ð¸Ð½Ð¸Ñ†Ð¸Ð°Ð»Ð¸Ð·Ð°Ñ†Ð¸Ñ info Ð´Ð°Ð½Ð½Ñ‹Ð¼Ð¸
	//ÐµÑÐ»Ð¸ info Ñ Ñ‚Ð°ÐºÐ¸Ð¼ id Ñ€Ð°Ð½ÑŒÑˆÐµ Ð½Ðµ Ð¸ÑÐ¿Ð¾Ð»ÑŒÐ·Ð¾Ð²Ð°Ð»ÑÑ
	//Ð¾Ð½ Ð±ÑƒÐ´ÐµÑ‚ Ð·Ð°Ð³Ñ€ÑƒÐ¶ÐµÐ½ Ð¸Ð· Ñ„Ð°Ð¹Ð»Ð°
	virtual void Load	(shared_str info_str_id);
	const ARTICLE_ID_VECTOR&						Articles	()	const {return info_data()->m_Articles;}
	const ARTICLE_ID_VECTOR&						ArticlesDisable	()	const {return info_data()->m_ArticlesDisable;}
	const DIALOG_ID_VECTOR&							DialogNames	()	const {return info_data()->m_DialogNames;}
	const SInfoPortionData::INFO_ID_VECTOR&			DisableInfos()	const {return info_data()->m_DisableInfo;}
	
			void									RunScriptActions		(const CGameObject* pOwner)	{info_data()->m_InfoScriptHelper.Action(pOwner, nullptr, NULL);}

	//Ñ‚ÐµÐºÑÑ‚Ð¾Ð²Ð¾Ðµ Ð¿Ñ€ÐµÐ´ÑÑ‚Ð°Ð²Ð»ÐµÐ½Ð¸Ðµ Ð¸Ð½Ñ„Ð¾Ñ€Ð¼Ð°Ñ†Ð¸Ð¸
			shared_str								GetText () const ;

protected:
    shared_str		m_InfoId;

	void			load_shared						(LPCSTR);
	SInfoPortionData* info_data						() { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}
	const SInfoPortionData* info_data				() const { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

	static void InitXmlIdToIndex();
};
*/
class CInfoPortion
{
	shared_str m_InfoId;
public:
	void Load(const shared_str& info_id) { m_InfoId = info_id; }
	const shared_str& InfoId() const { return m_InfoId; }
};
