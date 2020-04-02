#include "Beast/BeastAnimLandState.h"
#include "Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "ShellsCharacter.h"

void UBeastAnimLandState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		AShellsCharacter* Player = Cast<AShellsCharacter>(MeshComp->GetOwner());
		if (Player != nullptr)
		{
			Player->DisablePawnControl();
		}
	}
}

void  UBeastAnimLandState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		AShellsCharacter* Player = Cast<AShellsCharacter>(MeshComp->GetOwner());
		if (Player != nullptr)
		{
			Player->EnablePawnControl();
			if (Player->GetController())
			{
				Player->GetController()->SetIgnoreLookInput(false);
				Player->GetController()->SetIgnoreMoveInput(false);
			}
		}
	}
}