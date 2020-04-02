// Copyright 2019 - Filipe Ribeiro & Raul Leal


#include "MatchmakingRow.h"
#include "LeaderboardMenu.h"

void UMatchmakingRow::Setup(class ULeaderboardMenu* InParent, uint32 InIndex)
{
	Parent = InParent;
	Index = InIndex;
}

