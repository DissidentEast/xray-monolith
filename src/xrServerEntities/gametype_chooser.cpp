#include "stdafx.h"
#pragma hdrstop


#include "gametype_chooser.h"
#include "xrServer_Objects_Abstract.h"

//old
enum ERPGameType
{
	// [0..255]
	rpgtGameAny = u8(0),
	rpgtGameDeathmatch,
	rpgtGameTeamDeathmatch,
	rpgtGameArtefactHunt,
	rpgtGameCaptureTheArtefact,
	rpgtGameCount,
};

xr_token rpoint_game_type[] = {
	{"Any game", rpgtGameAny},
	{"Deathmatch", rpgtGameDeathmatch},
	{"TeamDeathmatch", rpgtGameTeamDeathmatch},
	{"ArtefactHunt", rpgtGameArtefactHunt},
	{"CaptureTheArtefact", rpgtGameCaptureTheArtefact},
	{0, 0}
};



#ifndef XRGAME_EXPORTS
void  GameTypeChooser::FillProp(LPCSTR pref, PropItemVec& items)
{
	PHelper().CreateGameType		(items, PrepareKey(pref, "Game Type"), this);
/*
    PHelper().CreateFlag16  (items, PrepareKey(pref, "Game Type\\single"),      			&m_GameType, eGameIDSingle);
	PHelper().CreateFlag16  (items, PrepareKey(pref, "Game Type\\deathmatch"),				&m_GameType, eGameIDDeathmatch);
    PHelper().CreateFlag16  (items, PrepareKey(pref, "Game Type\\team deathmatch"),     	&m_GameType, eGameIDTeamDeathmatch);
    PHelper().CreateFlag16  (items, PrepareKey(pref, "Game Type\\artefact hunt"),       	&m_GameType, eGameIDArtefactHunt);
    PHelper().CreateFlag16  (items, PrepareKey(pref, "Game Type\\capture the artefact"),	&m_GameType, eGameIDCaptureTheArtefact);
    PHelper().CreateFlag16  (items, PrepareKey(pref, "Game Type\\domination zone"),     	&m_GameType, eGameIDDominationZone);
    PHelper().CreateFlag16  (items, PrepareKey(pref, "Game Type\\team domination zone"),	&m_GameType, eGameIDTeamDominationZone);
*/
 }
#endif // #ifndef XRGAME_EXPORTS
