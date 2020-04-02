
#include "ExplosiveComponent.h"
#include "UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Engine/World.h"

UExplosiveComponent::UExplosiveComponent()
{
	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->Radius = 250;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false; // Use FireImpulse() instead of tick
	RadialForceComp->bIgnoreOwningActor = true;

	ExplosionImpulse = 400;

	SetIsReplicated(true);
}

void UExplosiveComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UExplosiveComponent::OnDamaged(class AController* InstigatedBy, AActor* DamageCauser)
{
	if (bExploded) { return; }

	bExploded = true;
	OnRep_Exploded();

	RadialForceComp->FireImpulse();
}


void UExplosiveComponent::OnRep_Exploded()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetOwner()->GetActorLocation());
}


void UExplosiveComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UExplosiveComponent, bExploded);
}