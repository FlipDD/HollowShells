
#include "BeamComponent.h"

#include "UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"

#include "Shells.h"
#include "Explorer/ExplorerCharacter.h"

UBeamComponent::UBeamComponent()
{
}

void UBeamComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UBeamComponent, HitScanTrace, COND_SkipOwner);
}

void UBeamComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBeamComponent::OnRep_HitScanTrace() const
{
	// Play cosmetic FX
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void UBeamComponent::Fire(FVector SpawnLocation, FRotator SpawnRotation)
{
	if (GetOwnerRole() < ROLE_Authority) {
		ServerFire(SpawnLocation, SpawnRotation);
	}
}


void UBeamComponent::ServerFire_Implementation(FVector SpawnLocation, FRotator SpawnRotation)
{
	Fire(SpawnLocation, SpawnRotation);
}


bool UBeamComponent::ServerFire_Validate(FVector SpawnLocation, FRotator SpawnRotation)
{
	return true;
}

void UBeamComponent::StartFire()
{
	GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &UBeamComponent::StopFire, Cooldown);
}


void UBeamComponent::StopFire()
{
	bCanShoot = true;
	GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}


void UBeamComponent::PlayFireEffects(FVector TraceEnd) const
{
	const auto Owner = Cast<AExplorerCharacter>(GetOwner());
	if (ensure(Owner)) {
		if (MuzzleEffect) {
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, Owner->GetMesh(), MuzzleSocketName);
		}

		if (TracerEffect) {
			const auto MuzzleLocation = Owner->GetMesh()->GetSocketLocation(MuzzleSocketName);
			auto TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
			if (TracerComp) {
				TracerComp->SetVectorParameter(TracerTargetName, TraceEnd);
			}
		}

		auto PC = Cast<APlayerController>(Owner->GetController());
		if (PC) {
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}
}

void UBeamComponent::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint) const
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

	const auto Owner = Cast<AExplorerCharacter>(GetOwner());
	if (ensure(Owner) && SelectedEffect) {
		const auto MuzzleLocation = Owner->GetMesh()->GetSocketLocation(MuzzleSocketName);
		auto ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

