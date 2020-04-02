#include "Beast/BeastAnimNotifyState.h"
#include "Components/SkeletalMeshComponent.h"
#include "ShellsCharacter.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

void UBeastAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		AShellsCharacter* Player = Cast<AShellsCharacter>(MeshComp->GetOwner());
		if (Player != nullptr)
		{
			Player->EnableCollisions();
		}
	}
}

void  UBeastAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		AShellsCharacter* Player = Cast<AShellsCharacter>(MeshComp->GetOwner());
		if (Player != nullptr)
		{
			Player->DisableCollisions();
		}
	}
}