// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryBaseUserWidget.h"

void UInventoryBaseWidget::SetItemData(FItemData Item)
{
	if (UIInventoryItem)
	{
		UIInventoryItem->SetItemName(Item.Name);
		UIInventoryItem->SetItemAmount(Item.Amount);
	}
}
