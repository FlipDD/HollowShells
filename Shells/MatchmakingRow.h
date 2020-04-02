// Copyright 2019 - Filipe Ribeiro & Raul Leal

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchmakingRow.generated.h"

/**
 * 
 */
UCLASS()
class SHELLS_API UMatchmakingRow : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* PlayerName;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* BPoints;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* BDeaths;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MPoints;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MDeaths;

	void Setup(class ULeaderboardMenu* InParent, uint32 InIndex);

	UPROPERTY()
		class ULeaderboardMenu* Parent;

	uint32 Index;
	
};
