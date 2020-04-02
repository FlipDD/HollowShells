
#include "MainPlayerState.h"
#include "UnrealNetwork.h"
#include "Session\HTTPService.h"
#include "Explorer/ExplorerCharacter.h"
#include "Beast/ShellsCharacter.h"

void AMainPlayerState::InitPlayerSession(AHTTPService* HTTPService,
	FString hash, int id, FString name, int beastpoints, int beastdeaths, int mechapoints, int mechadeaths) {
	HTTPServiceRef = HTTPService;
	AuthenticationHash = hash;
	PID = id;
	Name = name;
	TotalBeastPoints = beastpoints;
	TotalBeastDeaths = beastdeaths;
	TotalMechaPoints = mechapoints;
	TotalMechaDeaths = mechadeaths;
	UE_LOG(LogTemp, Warning, TEXT("Hash is /%s"), *hash);
	FString http = HTTPServiceRef->GetName();
	UE_LOG(LogTemp, Warning, TEXT("http name is: %s"), *http);

	AExplorerCharacter* Exp = Cast<AExplorerCharacter>(GetOwner());
	if (!Exp) return;

	Exp->PID = id;
	Exp->Name = name;
	Exp->TotalBeastDeaths = beastdeaths;
	Exp->TotalBeastPoints = beastpoints;
	Exp->TotalMechaDeaths = mechadeaths;
	Exp->TotalMechaPoints = mechapoints;
	
}

void AMainPlayerState::AddPlayerBeastPoint()
{
	TotalBeastPoints++;
	
	auto B = Cast<AShellsCharacter>(GetOwner());
	if (B) B->TotalBeastPoints = TotalBeastPoints;

	UE_LOG(LogTemp, Warning, TEXT("Added point"));

	if (HTTPServiceRef)
	{
		FAddBeastPointRequest AddPointInfo;
		AddPointInfo.hash = AuthenticationHash;
		AddPointInfo.userid = FString::FromInt(PID);
		HTTPServiceRef->AddBeastPoint(AddPointInfo);

		UE_LOG(LogTemp, Warning, TEXT("Add point info id : %d , hash %s"), *AddPointInfo.userid, *AddPointInfo.hash);
	}
}

void AMainPlayerState::AddPlayerBeastDeath()
{
	TotalBeastDeaths++;

	auto B = Cast<AShellsCharacter>(GetOwner());
	if (B) B->TotalBeastDeaths = TotalBeastDeaths;

	UE_LOG(LogTemp, Warning, TEXT("Added death"));

	if (HTTPServiceRef)
	{
		FAddBeastDeathRequest AddDeathInfo;
		AddDeathInfo.hash = AuthenticationHash;
		AddDeathInfo.userid = FString::FromInt(PID);
		HTTPServiceRef->AddBeastDeath(AddDeathInfo);

		UE_LOG(LogTemp, Warning, TEXT("Add point info id : %d , hash %s"), *AddDeathInfo.userid, *AddDeathInfo.hash);
	}
}

//mecha
void AMainPlayerState::AddPlayerMechaPoint()
{
	TotalMechaPoints++;

	auto B = Cast<AExplorerCharacter>(GetOwner());
	if (B) B->TotalMechaPoints = TotalMechaPoints;

	UE_LOG(LogTemp, Warning, TEXT("Added point"));

	if (HTTPServiceRef)
	{
		FAddMechaPointRequest AddPointInfo;
		AddPointInfo.hash = AuthenticationHash;
		AddPointInfo.userid = FString::FromInt(PID);
		HTTPServiceRef->AddMechaPoint(AddPointInfo);

		UE_LOG(LogTemp, Warning, TEXT("Add point info id : %d , hash %s"), *AddPointInfo.userid, *AddPointInfo.hash);
	}
}

void AMainPlayerState::AddPlayerMechaDeath()
{
	TotalMechaDeaths++;

	auto B = Cast<AExplorerCharacter>(GetOwner());
	if (B) B->TotalMechaDeaths = TotalMechaDeaths;

	UE_LOG(LogTemp, Warning, TEXT("Added death"));

	if (HTTPServiceRef)
	{
		FAddMechaDeathRequest AddDeathInfo;
		AddDeathInfo.hash = AuthenticationHash;
		AddDeathInfo.userid = FString::FromInt(PID);
		HTTPServiceRef->AddMechaDeath(AddDeathInfo);

		UE_LOG(LogTemp, Warning, TEXT("Add point info id : %d , hash %s"), *AddDeathInfo.userid, *AddDeathInfo.hash);
	}
}

void AMainPlayerState::GetLifetimeReplicatedProps(TArray<
	FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMainPlayerState, AuthenticationHash);
	DOREPLIFETIME(AMainPlayerState, TotalBeastPoints);
	DOREPLIFETIME(AMainPlayerState, TotalBeastDeaths);
	DOREPLIFETIME(AMainPlayerState, TotalMechaPoints);
	DOREPLIFETIME(AMainPlayerState, TotalMechaDeaths);
	DOREPLIFETIME(AMainPlayerState, PID);
}