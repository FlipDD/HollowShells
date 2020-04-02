// Copyright 2019 - Filipe Ribeiro & Raul Leal

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "BeastAnimEnableInputNotify.generated.h"

/**
 * 
 */
UCLASS()
class UBeastAnimEnableInputNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
