#include "HandController.h"
//#include "IPickable.h"
#include "GameFramework/PlayerController.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Runtime/Engine/Classes/Components/SplineMeshComponent.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"
#include "Runtime/HeadMountedDisplay/Public/IHeadMountedDisplay.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"

AHandController::AHandController() :
	Extents(500, 500, 500)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Hand = EControllerHand::Right;
	Grip = EGripState::Open;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	MotionController->SetupAttachment(Scene);
	MotionController->SetTrackingSource(Hand);

	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandMesh"));
	HandMesh->SetupAttachment(MotionController);

	GrabSphere = CreateDefaultSubobject<USphereComponent>(TEXT("GrabSphere"));
	GrabSphere->SetupAttachment(HandMesh);
	GrabSphere->InitSphereRadius(10.0f);
	GrabSphere->OnComponentBeginOverlap.AddDynamic(this, &AHandController::OnComponentBeginOverlap);
}

void AHandController::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (Hand == EControllerHand::Left)
	{
		// Reflect hand mesh
		HandMesh->SetWorldScale3D(FVector(1, 1, -1));
	}

}

void AHandController::RumbleController_Implementation(float intensity)
{
	FLatentActionInfo actionInfo;
	actionInfo.CallbackTarget = this;
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->PlayHapticEffect(HapticEffect, Hand, intensity, false);
}

void AHandController::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Grip = EGripState::CanGrab;
	if ((OtherComp != nullptr) && (OtherComp != GrabSphere))
	{
		UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(OtherComp);
		if (mesh && mesh->IsSimulatingPhysics())
		{
			this->RumbleController(0.8);
		}
	}
}

// Called when the game starts or when spawned
void AHandController::BeginPlay()
{
	Super::BeginPlay();

	MotionController->SetTrackingSource(Hand);
	if (Hand == EControllerHand::Left)
	{
		// Reflect hand mesh
		MotionController->SetTrackingSource(Hand);
		HandMesh->SetWorldScale3D(FVector(1, 1, -1));
	}
}

AActor* AHandController::GetActorNearHand()
{
	TArray<AActor*> overlappingActors;

	GrabSphere->GetOverlappingActors(overlappingActors);
	FVector handLocation = GrabSphere->GetComponentLocation();

	AActor* nearest = nullptr;
	float mindist = 99999999999;

	// Find closest overlaping actor
	for (AActor* actor : overlappingActors)
	{
		/*bool isPickupable = actor->GetClass()->ImplementsInterface(UPickupable::StaticClass());
		if (isPickupable)
		{

			float dist = (actor->GetActorLocation() - handLocation).SizeSquared();
			if (dist < mindist)
			{
				mindist = dist;
				nearest = actor;
			}
		}*/
	}

	return nearest;
}

void AHandController::UpdateAnimationGripState()
{
	Grip = EGripState::Open;

	if (AttachedActor)
	{
		Grip = EGripState::Grab;
	}
	else
	{
		if (WantsToGrip)
			Grip = EGripState::Grab;

		AActor* actor = GetActorNearHand();
		if (actor)
			Grip = EGripState::CanGrab;
	}

	// Only let hand collide with environment while gripping
	if (Grip == EGripState::Grab)
		HandMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	else
		HandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AHandController::GrabActor_Implementation()
{
	WantsToGrip = true;

	AActor* actor = GetActorNearHand();
	if (actor && actor->IsValidLowLevel())
	{
		AttachedActor = actor;
		//IPickable::Execute_Pickup(actor, MotionController);
		RumbleController(0.7);
	}

}

void AHandController::ReleaseActor_Implementation()
{
	WantsToGrip = false;

	AActor* actor = AttachedActor;
	if (actor && actor->IsValidLowLevel())
	{
		// Make sure this hand is still holding the Actor (May have been taken by another hand / event)
		if (MotionController == actor->GetRootComponent()->GetAttachParent())
		{
			//IPickupable::Execute_Drop(actor);
			RumbleController(0.2);
		}
	}

	AttachedActor = nullptr;
}

// Called every frame
void AHandController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAnimationGripState();

}

FRotator AHandController::GetControllerRelativeRotation()
{
	const FTransform InitialTransform(InitialControllerRotation);
	const FTransform CurrentTransform = MotionController->GetComponentTransform();
	const FTransform RelativeTransform = CurrentTransform.GetRelativeTransform(InitialTransform);

	return RelativeTransform.GetRotation().Rotator();
}