// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ExplorerProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "UnrealNetwork.h"
#include "Shells.h"
#include "Kismet/GameplayStatics.h"
#include "ExplorerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "MainPlayerState.h"
#include "Beast/ShellsCharacter.h"

AExplorerProjectile::AExplorerProjectile()
{
	// Use a sphere as a simple collision representation
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComponent->InitSphereRadius(5.0f);
	SphereComponent->BodyInstance.SetCollisionProfileName("Projectile");
	SphereComponent->OnComponentHit.AddDynamic(this, &AExplorerProjectile::OnHit);

	// Players can't walk on it
	SphereComponent->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	SphereComponent->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = SphereComponent;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = SphereComponent;
	ProjectileMovement->InitialSpeed = 4000.f;
	ProjectileMovement->MaxSpeed = 4000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

}

void AExplorerProjectile::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AExplorerProjectile, Instigator);
}

void AExplorerProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if (OtherActor && OtherActor != this && OtherComp)
	{
		auto Beast = Cast<AShellsCharacter>(OtherActor);
		if (Beast) {
			Beast->EnableBodyPhysics();
			auto State = Cast<AMainPlayerState>(Beast->GetPlayerState());
			if (State) State->AddPlayerBeastDeath();
			//auto MyState = Cast<AMainPlayerState>()
		}
		/*auto BeastComp = Cast<AShellsCharacter>(OtherComp->GetOwner());
		if (BeastComp) {
			BeastComp->EnableBodyPhysics();
		}*/

		if (OtherComp->IsSimulatingPhysics()) {
			OtherComp->AddImpulseAtLocation(GetVelocity() * 200.0f, GetActorLocation());
		}

		FString name = OtherComp->GetOwner()->GetName();
		UE_LOG(LogTemp, Warning, TEXT("hit actor name: %s"), *name);
		PlayImpactEffects(SURFACE_FLESHDEFAULT, Hit.ImpactPoint);
		SetLifeSpan(.2f);
	}
}

void AExplorerProjectile::SetInstigator(AExplorerCharacter *Player)
{
}

void AExplorerProjectile::ServerSetInstigator_Implementation(AExplorerCharacter *Player)
{
    
}

bool AExplorerProjectile::ServerSetInstigator_Validate(AExplorerCharacter *Player)
{
	return true;
}

void AExplorerProjectile::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint) const
{
	UParticleSystem* SelectedEffect = nullptr;
	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = DefaultImpactEffect;
		break;
	default:
		SelectedEffect = RockImpactEffect;
		break;
	}

	//const auto Owner = Cast<AExplorerCharacter>(GetOwner());
	//if (ensure(Owner) && SelectedEffect) {
	//	auto ShotDirection = Owner->GetVelocity();
	//	ShotDirection.Normalize();
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, FRotator::ZeroRotator);
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	//}
}

