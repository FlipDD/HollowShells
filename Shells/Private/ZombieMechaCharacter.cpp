#include "ZombieMechaCharacter.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "NetworkedGameState.h"
#include "Perception/PawnSensingComponent.h"
#include "UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"

AZombieMechaCharacter::AZombieMechaCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("SensingComponent");
	SensingComponent->OnSeePawn.AddDynamic(this, &AZombieMechaCharacter::SeePlayer);
	SensingComponent->SetSensingUpdatesEnabled(true);

	IsChasing = false;
	SetReplicates(true);
}

void AZombieMechaCharacter::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), Waypoints);
	AIController = Cast<AAIController>(GetController());
	this->bUseControllerRotationYaw = false;

	if ((Waypoints.Num() > 0) && (AIController))
	{
		MovementCompleteDelegate.BindUFunction(this, "AIMovementCompleted");
		AIController->ReceiveMoveCompleted.Add(MovementCompleteDelegate);
		AIController->MoveToActor(GetRandomWaypoint());
	}
}

void AZombieMechaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Target != nullptr && Role == ROLE_Authority)
	{
		if (FVector::Dist(GetActorLocation(), Target->GetActorLocation()) > SensingComponent->SightRadius)
		{
			GetMesh()->GlobalAnimRateScale = 1;
			this->GetCharacterMovement()->MaxWalkSpeed = 50;
			Target = nullptr; 
			AIController->MoveToActor(GetRandomWaypoint());
			IsChasing = false;
		}
	}
	if (IsChasing && GetMesh()->GlobalAnimRateScale != 2.5f)
	{
		GetMesh()->GlobalAnimRateScale = 2.5f;
		this->GetCharacterMovement()->MaxWalkSpeed = 150;
	}
	else if (!IsChasing && GetMesh()->GlobalAnimRateScale != 1.0f)
	{
		GetMesh()->GlobalAnimRateScale = 1;
		this->GetCharacterMovement()->MaxWalkSpeed = 50;
	}

}

void AZombieMechaCharacter::SeePlayer(APawn* Pawn)
{
	if ((Pawn) && (AIController) && (!Target) && (Role == ROLE_Authority)) //If not client?
	{
		Target = Pawn;
		AIController->MoveToActor(Target);
		IsChasing = true;
	}
}

ATargetPoint* AZombieMechaCharacter::GetRandomWaypoint()
{
	int index = FMath::RandRange(0, Waypoints.Num() - 1);
	return Cast<ATargetPoint>(Waypoints[index]);
}

void AZombieMechaCharacter::AIMovementCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result == EPathFollowingResult::Success)
	{
		AZombieMechaCharacter* Character = Cast<AZombieMechaCharacter>(Target);
		if (Character)
		{
			ANetworkedGameState* GameState = Cast<ANetworkedGameState>(GetWorld()->GetGameState());
			if (GameState)
			{
				GameState->MulticastOnLevelComplete(Character, false);
				Target = nullptr;
			}
		}
		if ((Waypoints.Num() > 0) && (AIController))
		{
			AIController->MoveToActor(GetRandomWaypoint());
		}
	}
}

void AZombieMechaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AZombieMechaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AZombieMechaCharacter, IsChasing);
}