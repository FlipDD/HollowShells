#include "Beast/BeastAnimCheckSwipeHitState.h"
#include "Components/SkeletalMeshComponent.h"
#include "ShellsCharacter.h"

void UBeastAnimCheckSwipeHitState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		auto Player = Cast<AShellsCharacter>(MeshComp->GetOwner());
		if (Player != nullptr)
		{
			Player->EnableClawCollision();
		}
	}
}

void  UBeastAnimCheckSwipeHitState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		auto Player = Cast<AShellsCharacter>(MeshComp->GetOwner());
		if (Player != nullptr)
		{
			Player->DisableClawCollision();
		}
	}
}