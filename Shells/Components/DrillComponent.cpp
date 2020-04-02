
#include "DrillComponent.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "UnrealNetwork.h"

#include "World/BeaconActor.h"

UDrillComponent::UDrillComponent()
{
	SetIsReplicated(true);
	PrimaryComponentTick.bCanEverTick = true;

	DrillSphereRadius = 200.0f;
	DrillSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	DrillSphere->SetupAttachment(GetAttachmentRoot());
	DrillSphere->SetSphereRadius(DrillSphereRadius);

	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>("ParticleComponent");
	ParticleComponent->SetupAttachment(GetAttachmentRoot());

	//DrillLoopSound = CreateDefaultSubobject<UAudioComponent>("DrillLoopSound");
	//DrillLoopSound->SetupAttachment(GetAttachmentRoot());
	//DrillLoopSound->bAutoActivate = false;
}

void UDrillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDrillComponent, CurrentState);
}


void UDrillComponent::OnRep_CurrentState()
{
	switch (CurrentState) {
	case EDrillState::Idle:
		if (ParticleComponent)
			ParticleComponent->DeactivateSystem();
		break;
	case EDrillState::Drilling:
		if (ParticleComponent)
			ParticleComponent->ActivateSystem();
		break;
	default:;
	}
}

void UDrillComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UDrillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDrillComponent::StartDrilling()
{
	UE_LOG(LogTemp, Warning, TEXT("StartDrilling"));
	ServerStartDrilling();
}

void UDrillComponent::ServerStartDrilling()
{
	UE_LOG(LogTemp, Warning, TEXT("ServerStartDrilling"));

	TArray<AActor*> Found;
	DrillSphere->GetOverlappingActors(Found);

	for (auto A : Found) {
		const auto Beacon = Cast<ABeaconActor>(A);
		if (Beacon) {
			CurrentState = EDrillState::Drilling;
			UE_LOG(LogTemp, Warning, TEXT("Drilling Beacon"));
			Beacon->BeginDrilling();
			break;
		}
	}
}

void UDrillComponent::StopDrilling()
{
	CurrentState = EDrillState::Idle;
}

