// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "World/Item.h"
#include "InventoryItemUserWidget.h"
#include "InventoryBaseUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class SHELLS_API UInventoryBaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UInventoryItemWidget* UIInventoryItem;

	void SetItemData(FItemData Item);
};
