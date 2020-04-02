// Copyright 2019 - Filipe Ribeiro & Raul Leal

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BeastInGameUI.generated.h"

/**
 * 
 */
UCLASS()
class UBeastInGameUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOut;
	
};
