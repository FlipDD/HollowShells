
#include "LauncherComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Shells.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Explorer/ExplorerCharacter.h"

#include "HeadMountedDisplayFunctionLibrary.h"


static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("Shells.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for Weapons"),
	ECVF_Cheat);

ULauncherComponent::ULauncherComponent()
{
	SetIsReplicated(true);
	PrimaryComponentTick.bCanEverTick = true;

	MuzzleSocketName = "RightHand";
	Spread = 2.0f;
	Cooldown = 3.0f;
}

void ULauncherComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ULauncherComponent, bCanShoot);
}

void ULauncherComponent::BeginPlay()
{
	Super::BeginPlay();
}


void ULauncherComponent::Fire(FVector SpawnLocation, FRotator SpawnRotation, USceneComponent* ComponentToAttach)
{
	/*if (GetOwnerRole() < ROLE_Authority) {*/
		ServerFire(SpawnLocation, SpawnRotation, ComponentToAttach);
	//}

	PlayFireEffects(ComponentToAttach);
}


void ULauncherComponent::ServerFire_Implementation(FVector SpawnLocation, FRotator SpawnRotation, USceneComponent* Component)
{
	MulticastFire(SpawnLocation, SpawnRotation, Component);
}


bool ULauncherComponent::ServerFire_Validate(FVector SpawnLocation, FRotator SpawnRotation, USceneComponent* Component)
{
	return true;
}

void ULauncherComponent::MulticastFire_Implementation(FVector SpawnLocation, FRotator SpawnRotation, USceneComponent* Component)
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	const auto World = GetWorld();
	FRotator Direction;
	if (Component != NULL) {
		Direction = Component->GetComponentRotation();
	}
	else {
		Direction = SpawnRotation;
	}
	World->SpawnActor<AExplorerProjectile>(ProjectileClass, SpawnLocation, Direction, ActorSpawnParams);
	UE_LOG(LogTemp, Warning, TEXT("SPAWNED PROJECTILE"));
}

bool ULauncherComponent::MulticastFire_Validate(FVector SpawnLocation, FRotator SpawnRotation, USceneComponent* Component)
{
	return true;
}

void ULauncherComponent::StartFire()
{
	bCanShoot = false;
	GetOwner()->GetWorldTimerManager().SetTimer(TimerHandle_Cooldown, this, &ULauncherComponent::StopFire, Cooldown);
}


void ULauncherComponent::StopFire()
{
	if (GetOwnerRole() < ROLE_Authority) return;

	GetOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_Cooldown);
	bCanShoot = true;
}


void ULauncherComponent::PlayFireEffects(USceneComponent* ComponentToAttach) const
{
	const auto Owner = Cast<AExplorerCharacter>(GetOwner());
	if (ensure(Owner)) {
		if (MuzzleEffect) {
			if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled()) {
				UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, ComponentToAttach, FName(NAME_None), FVector(ForceInit), FRotator::ZeroRotator, FVector(.5f));
			}
			else {
				UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, Owner->GetMesh(), MuzzleSocketName);
			}
		}

		auto PC = Cast<APlayerController>(Owner->GetController());
		if (PC) {
			PC->ClientPlayCameraShake(CamShake);
		}
	}
}
