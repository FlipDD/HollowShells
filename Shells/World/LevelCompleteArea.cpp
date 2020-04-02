
#include "LevelCompleteArea.h"
#include "Components/BoxComponent.h"

ALevelCompleteArea::ALevelCompleteArea()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	BoxComponent->SetBoxExtent(FVector(200, 200, 200));
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComponent->SetupAttachment(RootComponent);

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ALevelCompleteArea::HandleBeginOverlap);
}

void ALevelCompleteArea::HandleBeginOverlap(
	UPrimitiveComponent * OverlappedComponent,
	AActor * OtherActor,
	UPrimitiveComponent * OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("OVERLAP"));
}

void ALevelCompleteArea::BeginPlay()
{
	Super::BeginPlay();
}

void ALevelCompleteArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

