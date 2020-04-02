#include "BeastAnimEnableInputNotify.h"
#include "Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "ShellsCharacter.h"

void UBeastAnimEnableInputNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		AShellsCharacter* Player = Cast<AShellsCharacter>(MeshComp->GetOwner());
		if (Player != nullptr)
		{
			AController* C = Player->GetController();
			if (C != nullptr)
			{
				C->SetIgnoreLookInput(false);
				C->SetIgnoreMoveInput(false);
				Player->EnablePawnControl();
			}
		}
	}
}
