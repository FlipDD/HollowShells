// Copyright 2019 - Filipe Ribeiro & Raul Leal

#pragma once

#include "CoreMinimal.h"
#include "UI/MenuWidget.h"
#include "Animation/WidgetAnimation.h"
#include "RespawnScreen.generated.h"

UCLASS()
class URespawnScreen : public UMenuWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidgetAnim))
	UWidgetAnimation* FadeIn;

};
