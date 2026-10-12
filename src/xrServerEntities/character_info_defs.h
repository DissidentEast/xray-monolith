#pragma once

#include "alife_space.h"


//Ð»Ð¸Ñ‡Ð½Ð¾Ðµ Ð¾Ñ‚Ð½Ð¾ÑˆÐµÐ½Ð¸Ðµ (Ð±Ð»Ð°Ð³Ð¾ÑÐºÐ»Ð¾Ð½Ð½Ð¾ÑÑ‚ÑŒ) Ð¾Ð´Ð½Ð¾Ð³Ð¾ Ð¿ÐµÑ€ÑÐ¾Ð½Ð°Ð¶Ð° Ðº Ð´Ñ€ÑƒÐ³Ð¾Ð¼Ñƒ - 
//Ð²ÐµÐ»Ð¸Ñ‡Ð¸Ð½Ð° Ð¾Ñ‚ -100< (ÐºÑ€Ð°Ð¹Ð½Ðµ Ð²Ñ€Ð°Ð¶Ð´ÐµÐ±Ð½Ð¾Ðµ) Ð´Ð¾ >100 (Ð¾Ñ‡ÐµÐ½ÑŒ Ð´Ñ€ÑŽÐ¶ÐµÐ»ÑŽÐ±Ð½Ð¾Ðµ)
typedef int CHARACTER_GOODWILL;
#define NO_GOODWILL				-type_max(CHARACTER_GOODWILL)
#define NEUTRAL_GOODWILL		CHARACTER_GOODWILL(0)

typedef shared_str CHARACTER_CLASS;
#define NO_CHARACTER_CLASS		NULL

//Ñ€ÐµÐ¿ÑƒÑ‚Ð°Ñ†Ð¸Ñ Ð¿ÐµÑ€ÑÐ¾Ð½Ð°Ð¶Ð° - Ð²ÐµÐ»Ð¸Ñ‡Ð¸Ð½Ð° Ð¾Ñ‚ -100 (Ð¾Ñ‡ÐµÐ½ÑŒ Ð¿Ð»Ð¾Ñ…Ð¾Ð¹, Ð±ÐµÑÐ¿Ñ€ÐµÐ´ÐµÐ»ÑŒÑ‰Ð¸Ðº) 
//Ð´Ð¾ 100 (Ð¾Ñ‡ÐµÐ½ÑŒ Ñ…Ð¾Ñ€Ð¾ÑˆÐ¸Ð¹, Ð±Ð»Ð°Ð³Ð¾Ñ€Ð¾Ð´Ð½Ñ‹Ð¹)
typedef int CHARACTER_REPUTATION_VALUE;
#define NO_REPUTATION			-type_max(CHARACTER_REPUTATION_VALUE)
#define NEUTAL_REPUTATION		0

//Ñ€Ð°Ð½Ð³ Ð¿ÐµÑ€ÑÐ¾Ð½Ð°Ð¶Ð° - Ð²ÐµÐ»Ð¸Ñ‡Ð¸Ð½Ð° Ð¾Ñ‚ 0 (ÑÐ¾Ð²ÑÐµÐ¼ Ð½ÐµÐ¾Ð¿Ñ‹Ñ‚Ð½Ñ‹Ð¹) 
//Ð´Ð¾ >100 (Ð¾Ñ‡ÐµÐ½ÑŒ Ð¾Ð¿Ñ‹Ñ‚Ð½Ñ‹Ð¹)
typedef int CHARACTER_RANK_VALUE;
#define NO_RANK					-type_max(CHARACTER_RANK_VALUE)


typedef shared_str CHARACTER_COMMUNITY_ID;
#define NO_COMMUNITY_ID			CHARACTER_COMMUNITY_ID(nullptr)

typedef int CHARACTER_COMMUNITY_INDEX;
#define NO_COMMUNITY_INDEX		CHARACTER_COMMUNITY_INDEX(-1)
