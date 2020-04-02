#include "MovingRock.h"

#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

AMovingRock::AMovingRock()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);
	
	RumbleSound = CreateDefaultSubobject<UAudioComponent>("AudioComponent");
	RumbleSound->SetupAttachment(RootComponent);

	Speed = 15;
	CanPlay = true;
}

void AMovingRock::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}

	GlobalStartLocation = GetActorLocation();
	GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);

	RumbleSound->Stop();
}

void AMovingRock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		FVector Location = GetActorLocation();

		if (ActiveTriggers >= TargetTriggers)
		{
			float JourneyLength = (GlobalTargetLocation - GlobalStartLocation).Size();
			float JourneyTraveled = (Location - GlobalStartLocation).Size();

			if (JourneyTraveled <= JourneyLength)
			{
				FVector Direction = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal();
				Location += Direction * Speed * DeltaTime;
				SetActorLocation(Location);
				bIsMoving = true;
			}
			else
			{
				bIsMoving = false;
			}
		}
		else if (ActiveTriggers < TargetTriggers && ActiveTriggers > -6)
		{
			float DistanceFromStart = FVector::Dist(GlobalStartLocation, Location);
			if (DistanceFromStart > 10)
			{
				FVector Direction = (GlobalStartLocation - Location).GetSafeNormal();
				Location += Direction * Speed * DeltaTime;
				SetActorLocation(Location);
				bIsMoving = true;
			}
			else
			{
				bIsMoving = false;
			}
		}
		else if (ActiveTriggers <= -6)
		{
			float DistanceFromTarget = FVector::Dist(GlobalTargetLocation, Location);
			if (DistanceFromTarget > 10)
			{
				FVector Direction = (GlobalTargetLocation - Location).GetSafeNormal();
				Location += Direction * Speed * DeltaTime;
				SetActorLocation(Location);
				bIsMoving = false;
			}
			else
			{
				bIsMoving = false;
				this->SetActorTickEnabled(false);
			}
		}
	}
}

void AMovingRock::AddActiveTrigger()
{
	ActiveTriggers++;
}

void AMovingRock::RemoveActiveTrigger()
{
	if (ActiveTriggers > 0)
	{
		ActiveTriggers--;
	}
}

void AMovingRock::Unlock()
{
	ActiveTriggers = -10;
}

void AMovingRock::OnRep_IsMoving()
{
	if (bIsMoving) {
		RumbleSound->Play();
	}
	else {
		RumbleSound->Stop();
	}
}

void AMovingRock::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMovingRock, bIsMoving);
}
