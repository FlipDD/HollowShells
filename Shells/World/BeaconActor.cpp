#include "World/BeaconActor.h"
#include "DestructibleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Particles/ParticleSystemComponent.h"
#include "Net/UnrealNetwork.h"


ABeaconActor::ABeaconActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	MeshComponent->SetupAttachment(RootComponent);

	Root1Component = CreateDefaultSubobject<UStaticMeshComponent>("Root1");
	Root1Component->SetupAttachment(MeshComponent);
	Root1Component->SetRelativeLocation(FVector(-5, -10, 3.5f));
	Root1Component->SetRelativeRotation(FQuat(FVector(0, 0, 1), -70));
	Root1Component->SetRelativeScale3D(FVector(.45f, .45f, .3f));
	Root2Component = CreateDefaultSubobject<UStaticMeshComponent>("Root2");
	Root2Component->SetupAttachment(MeshComponent);
	Root2Component->SetRelativeLocation(FVector(-1.5f, -10, 3.5f));
	Root2Component->SetRelativeRotation(FQuat(FVector(0, 0, 1), 50));
	Root2Component->SetRelativeScale3D(FVector(.45f, .45f, .3f));
	Root3Component = CreateDefaultSubobject<UStaticMeshComponent>("Root3");
	Root3Component->SetupAttachment(MeshComponent);
	Root3Component->SetRelativeLocation(FVector(-1.5, .15f, 3.5f));
	Root3Component->SetRelativeRotation(FQuat(FVector(0, 0, 1), 170));
	Root3Component->SetRelativeScale3D(FVector(.45f, .45f, .3f));

	SphereComponent = CreateDefaultSubobject<UStaticMeshComponent>("SphereComponent");
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetRelativeLocation(FVector(0, 0, -130));
	SphereComponent->SetRelativeScale3D(FVector(1.2f));

	BeaconExplosionComponent = CreateDefaultSubobject<UParticleSystemComponent>("ExplosionParticles");
	BeaconExplosionComponent->SetupAttachment(RootComponent);
	BeaconExplosionComponent->SetRelativeLocation(FVector(0, 0, 30));
	BeaconExplosionComponent->SetRelativeScale3D(FVector(2));
	BeaconExplosionComponent->SetAutoActivate(false);
	BeaconEnergyComponent = CreateDefaultSubobject<UParticleSystemComponent>("EnergyParticles");
	BeaconEnergyComponent->SetupAttachment(RootComponent);
	BeaconEnergyComponent->SetRelativeLocation(FVector(0, 0, 40));
	BeaconEnergyComponent->SetRelativeScale3D(FVector(3));

	DestructibleMesh = CreateDefaultSubobject<UDestructibleComponent>("DestructibleMesh");
	DestructibleMesh->SetupAttachment(RootComponent);
	DestructibleMesh->SetVisibility(false);
	DestructibleMesh->SetSimulatePhysics(false);
	/*DestructibleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DestructibleMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);*/
	DestructibleGround = CreateDefaultSubobject<UDestructibleComponent>("DestructibleGround");
	DestructibleGround->SetupAttachment(RootComponent);
	DestructibleGround->SetVisibility(false);
	DestructibleGround->SetSimulatePhysics(false);
	
	// Item Collection
	DrillSphereRadius = 300.0f;
	DrillSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DrillSphere"));
	DrillSphere->SetupAttachment(RootComponent);
	DrillSphere->SetSphereRadius(DrillSphereRadius);

	WasDestroyed = false;
	DestructionProgress = 0;

	SetReplicates(true);
}

void ABeaconActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABeaconActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ExplorersDrilling > 0)
	{
		Progress -= DeltaTime;
		if (Progress)
		{
			UE_LOG(LogTemp, Warning, TEXT("Progressing %d"), Progress);
			OnDrillingSuccess();
		}
	}
}


void ABeaconActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABeaconActor, Progress);
	DOREPLIFETIME(ABeaconActor, ExplorersDrilling);
}

void ABeaconActor::BeginDrilling()
{
	MulticastBeginDrill();
	ServerBeginDrilling();
}

void ABeaconActor::ServerBeginDrilling_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ServerBeginDrilling_Implementation"));

	MulticastBeginDrill();
}

bool ABeaconActor::ServerBeginDrilling_Validate()
{
	return true;
}

void ABeaconActor::MulticastBeginDrill_Implementation()
{
	ExplorersDrilling += 1;
}
bool ABeaconActor::MulticastBeginDrill_Validate()
{
	return true;
}

void ABeaconActor::MulticastEndDrill_Implementation()
{
	ExplorersDrilling -= 1;
}
bool ABeaconActor::MulticastEndDrill_Validate()
{
	return true;
}



void ABeaconActor::ServerStopDrilling_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ServerBeginDrilling_Implementation"));

	MulticastEndDrill();

	//bIsBeingDrilled = false;
}

bool ABeaconActor::ServerStopDrilling_Validate()
{
	return true;
}

bool ABeaconActor::OnDrillingSuccess_Validate() { return true; }
void ABeaconActor::OnDrillingSuccess_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("OnDrillingSuccess_Implementation"));

	MeshComponent->SetVisibility(false);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	/*TArray<USceneComponent*> Roots;
	MeshComponent->GetChildrenComponents(false, Roots);*/
	//for (auto r : Roots)
	//{
	//	auto Mesh = Cast<UStaticMeshComponent>(r);
	//	if (Mesh)
	//	{
	//		UMaterialInstanceDynamic* Mat = Mesh->CreateDynamicMaterialInstance(0, GlowingMaterialInst);
	//		// Mat->SetVectorParameterValue(FName("OverrideColor"), FLinearColor::Black);
	//	}
	//}

	DestructibleMesh->SetVisibility(true);
	DestructibleMesh->SetSimulatePhysics(true);
	DestructibleMesh->AddRadialImpulse(DestructibleMesh->GetComponentLocation(), 100, PillarImpulseForce, ERadialImpulseFalloff::RIF_Linear, true);
	//DestructibleMesh->ApplyDamage(1000, DestructibleMesh->GetComponentLocation(), DestructibleMesh->GetUpVector() * -1, 10000);
	//DestructibleMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//DestructibleMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	DestructibleGround->SetVisibility(true);
	DestructibleGround->SetSimulatePhysics(true);
	DestructibleGround->AddRadialImpulse(DestructibleGround->GetComponentLocation(), 500, GroundImpulseForce, ERadialImpulseFalloff::RIF_Linear, true);

	SphereComponent->SetSimulatePhysics(true);
	SphereComponent->AddImpulse(SphereComponent->GetUpVector() * SphereSpeed, NAME_None, true);
	BeaconExplosionComponent->Activate(true);

	// Effects
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeaconLight, GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(this, DestroySound, GetActorLocation(), SoundMultiplier);
	UGameplayStatics::PlaySoundAtLocation(this, RumbleSound, GetActorLocation(), SoundMultiplier);

	SetActorTickEnabled(false);
}

bool ABeaconActor::OnDrillingFailure_Validate() { return true; }
void ABeaconActor::OnDrillingFailure_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("OnDrillingFailure_Implementation"));
}


void ABeaconActor::SetDestroyed()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}


