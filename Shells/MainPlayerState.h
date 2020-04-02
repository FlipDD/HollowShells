#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LeaderboardMenu.h"
#include "MainPlayerState.generated.h"

/**
 *
 */
UCLASS()
class SHELLS_API AMainPlayerState : public APlayerState
{
	GENERATED_BODY()

private:
	class AHTTPService* HTTPServiceRef;

public:
	UPROPERTY(Replicated)
	FString AuthenticationHash;

	UPROPERTY(Replicated)
		int PID;
	UPROPERTY(Replicated)
		FString Name;
	UPROPERTY(Replicated)
		int TotalBeastPoints;
	UPROPERTY(Replicated)
		int TotalBeastDeaths;
	UPROPERTY(Replicated)
		int TotalMechaPoints;
	UPROPERTY(Replicated)
		int TotalMechaDeaths;
	
	// 1 means , 2 means, 3 means ...-
	//UPROPERTY(Replicated)
	//	int ModifierItem;

public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	void InitPlayerSession(AHTTPService* HTTPService, FString hash, int id, FString name,
		int beastpoints, int beastdeaths, int mechapoints, int mechadeaths);

	void AddDeath(int score);
	void AddKill(int score);
	void AddScore(int score);

	UFUNCTION(BlueprintCallable)
	void AddPlayerBeastPoint();

	UFUNCTION(BlueprintCallable)
	void AddPlayerBeastDeath();

	UFUNCTION(BlueprintCallable)
		void AddPlayerMechaPoint();

	UFUNCTION(BlueprintCallable)
		void AddPlayerMechaDeath();

	int GetScore() const { return Score; };
	int GetKills() const { return Kills; };
	int GetDeaths() const { return Deaths; };


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	ULeaderboardMenu* Leaderboard;

private:
	int Score;
	int Deaths;
	int Kills;
};
