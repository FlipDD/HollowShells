#include "World/PortalBox.h"
#include "Beast/ShellsCharacter.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "DrawDebugHelpers.h"

APortalBox::APortalBox()
{
	//Register Events
	//OnActorBeginOverlap.AddDynamic(this, &APortalBox::OnOverlapBegin);
	//OnActorEndOverlap.AddDynamic(this, &APortalBox::OnOverlapEnd);
}

void APortalBox::BeginPlay()
{
	Super::BeginPlay();

	//DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Purple, true, -1, 0, 5);
}

//void APortalBox::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
//{
//	if (OtherActor && OtherActor != this) {
//		auto BeastCharacter = Cast<AShellsCharacter>(OtherActor);
//		if (BeastCharacter) {
//			if (DestinationPortal) {
//				const auto Pos = DestinationPortal->GetActorLocation();
//				BeastCharacter->OnOverlapPortal(Pos, true);
//			}
//		}
//	}
//}
//
//void APortalBox::OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor)
//{
//	if (OtherActor && OtherActor != this) {
//		auto BeastCharacter = Cast<AShellsCharacter>(OtherActor);
//		if (BeastCharacter) {
//			if (DestinationPortal) {
//				const auto Pos = DestinationPortal->GetActorLocation();
//				BeastCharacter->OnOverlapPortal(Pos, false);
//			}
//		}
//	}
//}

