// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionRow.generated.h"

/**
 *
 */
UCLASS()
class USessionRow : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
		class UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* RefreshButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* OptionsButton;
};
