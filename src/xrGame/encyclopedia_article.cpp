///////////////////////////////////////////////////////////////
// encyclopedia_article.cpp
// ÑÑ‚Ñ€ÑƒÐºÑ‚ÑƒÑ€Ð°, Ñ…Ñ€Ð°Ð½ÑÑ‰Ð°Ñ Ð¸ Ð·Ð°Ð³Ñ€ÑƒÐ¶Ð°ÑŽÑ‰Ð°Ñ ÑÑ‚Ð°Ñ‚ÑŒÐ¸ Ð² ÑÐ½Ñ†Ð¸ÐºÐ»Ð¾Ð¿ÐµÐ´Ð¸ÑŽ
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "encyclopedia_article.h"
#include "ui/xrUIXmlParser.h"
#include "ui/UIXmlInit.h"
#include "ui/UIInventoryUtilities.h"
#include "object_broker.h"
#include "../Include/xrRender/UIShader.h"

using namespace InventoryUtilities;

void ARTICLE_DATA::load(IReader& stream)
{
	load_data(receive_time, stream);
	load_data(article_id, stream);
	load_data(readed, stream);
	load_data(article_type, stream);
}

void ARTICLE_DATA::save(IWriter& stream)
{
	save_data(receive_time, stream);
	save_data(article_id, stream);
	save_data(readed, stream);
	save_data(article_type, stream);
}

CEncyclopediaArticle::CEncyclopediaArticle()
{
}

CEncyclopediaArticle::~CEncyclopediaArticle()
{
	if (data()->image.GetParent())
		data()->image.GetParent()->DetachChild(&(data()->image));
}

/*
void CEncyclopediaArticle::Load	(ARTICLE_STR_ID str_id)
{
	Load	(id_to_index::IdToIndex(str_id));
}
*/
void CEncyclopediaArticle::Load(shared_str id)
{
	m_ArticleId = id;
	inherited_shared::load_shared(m_ArticleId, nullptr);
}


void CEncyclopediaArticle::load_shared(LPCSTR)
{
	const ITEM_DATA& item_data = *id_to_index::GetById(m_ArticleId);

	CUIXml* pXML = item_data._xml;
	pXML->SetLocalRoot(pXML->GetRoot());

	//loading from XML
	XML_NODE* pNode = pXML->NavigateToNode(id_to_index::tag_name, item_data.pos_in_file);
	THROW3(pNode, "encyclopedia article id=", *item_data.id);

	//Ñ‚ÐµÐºÑÑ‚
	data()->text = pXML->Read(pNode, "text", 0, "");
	//Ð¸Ð¼Ñ
	data()->name = pXML->ReadAttrib(pNode, "name", "");
	//Ð³Ñ€ÑƒÐ¿Ð¿Ð°
	data()->group = pXML->ReadAttrib(pNode, "group", "");
	//ÑÐµÐºÑ†Ð¸Ñ ltx, Ð¾Ñ‚ÐºÑƒÐ´Ð° Ñ‡Ð¸Ñ‚Ð°Ñ‚ÑŒ Ð´Ð°Ð½Ð½Ñ‹Ðµ
	LPCSTR ltx = pXML->Read(pNode, "ltx", 0, nullptr);


	if (ltx)
	{
		if (pSettings->line_exist(ltx, "icons_texture"))
		{
			LPCSTR icons_texture = pSettings->r_string(ltx, "icons_texture");
			data()->image.SetShader(InventoryUtilities::GetCustomIconTextureShader(icons_texture));
		}
		else
			data()->image.SetShader(InventoryUtilities::GetEquipmentIconsShader());
		
		Frect tex_rect;
		tex_rect.x1 = float(pSettings->r_u32(ltx, "inv_grid_x") * INV_GRID_WIDTH);
		tex_rect.y1 = float(pSettings->r_u32(ltx, "inv_grid_y") * INV_GRID_HEIGHT);
		tex_rect.x2 = float(pSettings->r_u32(ltx, "inv_grid_width") * INV_GRID_WIDTH);
		tex_rect.y2 = float(pSettings->r_u32(ltx, "inv_grid_height") * INV_GRID_HEIGHT);
		tex_rect.rb.add(tex_rect.lt);
		data()->image.GetUIStaticItem().SetTextureRect(tex_rect);
	}
	else
	{
		if (pXML->NavigateToNode(pNode, "texture", 0))
		{
			pXML->SetLocalRoot(pNode);
			CUIXmlInit::InitTexture(*pXML, "", 0, &data()->image);
			pXML->SetLocalRoot(pXML->GetRoot());
		}
	}

	if (data()->image.GetShader() && data()->image.GetShader()->inited())
	{
		Frect r = data()->image.GetUIStaticItem().GetTextureRect();
		data()->image.SetAutoDelete(false);

		const int minSize = 65;

		// Ð¡Ð½Ð°Ñ‡Ð°Ð»Ð° ÑƒÑÑ‚Ð°Ð½Ð°Ð²Ð»Ð¸Ð²Ð°ÐµÐ¼ ÐµÑÐ»Ð¸ Ð½Ð°Ð´Ð¾ Ð¼Ð¸Ð½Ð¸Ð¼Ð°Ð»ÑŒÐ½Ð¾ Ð´Ð¾Ð¿ÑƒÑÑ‚Ð¸Ð¼Ñ‹Ðµ Ñ€Ð°Ð·Ð¼ÐµÑ€Ñ‹ Ð¸ÐºÐ¾Ð½ÐºÐ¸
		if (r.width() < minSize)
		{
			float dx = minSize - r.width();
			r.x2 += dx;
			data()->image.SetTextureOffset(dx / 2, data()->image.GetTextureOffeset()[1]);
		}

		if (r.height() < minSize)
		{
			float dy = minSize - r.height();
			r.y2 += dy;
			data()->image.SetTextureOffset(data()->image.GetTextureOffeset()[0], dy / 2);
		}

		data()->image.SetWndRect(Frect().set(0, 0, r.width(), r.height()));
	};

	// Ð¢Ð¸Ð¿ ÑÑ‚Ð°Ñ‚ÑŒÐ¸
	xr_string atricle_type = pXML->ReadAttrib(pNode, "article_type", "encyclopedia");
	if (0 == stricmp(atricle_type.c_str(), "encyclopedia"))
	{
		data()->articleType = ARTICLE_DATA::eEncyclopediaArticle;
	}
	else if (0 == stricmp(atricle_type.c_str(), "journal"))
	{
		data()->articleType = ARTICLE_DATA::eJournalArticle;
	}
	else if (0 == stricmp(atricle_type.c_str(), "task"))
	{
		data()->articleType = ARTICLE_DATA::eTaskArticle;
	}
	else if (0 == stricmp(atricle_type.c_str(), "info"))
	{
		data()->articleType = ARTICLE_DATA::eInfoArticle;
	}
	else
	{
		Msg("incorrect article type definition for [%s]", *item_data.id);
	}

	data()->ui_template_name = pXML->ReadAttrib(pNode, "ui_template", "common");
}

void CEncyclopediaArticle::InitXmlIdToIndex()
{
	if (!id_to_index::tag_name)
		id_to_index::tag_name = "article";
	if (!id_to_index::file_str)
		id_to_index::file_str = pSettings->r_string("encyclopedia", "files");
}
