#include "ToggleMovementNotifyState.h"

#include "Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

#include "ShellsCharacter.h"
#include "Explorer/ExplorerCharacter.h"

void UToggleMovementNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		auto Player = Cast<AShellsCharacter>(MeshComp->GetOwner());
		if (Player != nullptr)
		{
			Player->GetCharacterMovement()->MaxWalkSpeed = 0;
		}
		else
		{
			auto Explorer = Cast<AExplorerCharacter>(MeshComp->GetOwner());
			if (Explorer != nullptr)
			{
				Explorer->GetCharacterMovement()->MaxWalkSpeed = 0;
			}
		}
	}
}

void  UToggleMovementNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		auto Player = Cast<AShellsCharacter>(MeshComp->GetOwner());
		if (Player != nullptr)
		{
			Player->GetCharacterMovement()->MaxWalkSpeed = 600;
		}
		else
		{
			auto Explorer = Cast<AExplorerCharacter>(MeshComp->GetOwner());
			if (Explorer != nullptr)
			{
				Explorer->GetCharacterMovement()->MaxWalkSpeed = 300;
			}
		}
	}
}

