#include "CheckSwipeRecoverNotify.h"
#include "Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "ShellsCharacter.h"

void UCheckSwipeRecoverNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp != nullptr && MeshComp->GetOwner() != nullptr)
	{
		AShellsCharacter* Player = Cast<AShellsCharacter>(MeshComp->GetOwner());
		if (Player != nullptr)
		{
			Player->CheckIfSwipeHit();
		}
	}
}




