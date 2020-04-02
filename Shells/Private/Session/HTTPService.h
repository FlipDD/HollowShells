
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Online/HTTP/public/Http.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "HTTPService.generated.h"

USTRUCT()
struct FLoginRequest 
{
	GENERATED_BODY()

	UPROPERTY()
		FString email;
	UPROPERTY()
		FString	password;
};


USTRUCT()
struct FLoginData
{
	GENERATED_BODY()

	UPROPERTY()
		int status;
	UPROPERTY()
		int userid;
	UPROPERTY()
		FString name;
	UPROPERTY()
		FString hash;
	UPROPERTY()
		int totalbeastpoints;
	UPROPERTY()
		int totalbeastdeaths;
	UPROPERTY()
		int totalmechapoints;
	UPROPERTY()
		int totalmechadeaths;
};

USTRUCT()
struct FLoginResult
{
	GENERATED_BODY()

	UPROPERTY()
		FLoginData LoginResult;
};

USTRUCT()
struct FAddBeastPointRequest
{
	GENERATED_BODY()

	UPROPERTY()
		FString userid;
	UPROPERTY()
		FString hash;
};

USTRUCT()
struct FAddBeastPointData
{
	GENERATED_BODY()

	UPROPERTY()
		int status;
};

USTRUCT()
struct FResponseAddBeastPoint
{
	GENERATED_BODY();

	UPROPERTY()
		FAddBeastPointData AddUserBeastPointResult;
};

//mecha
USTRUCT()
struct FAddMechaPointRequest
{
	GENERATED_BODY()

	UPROPERTY()
		FString userid;
	UPROPERTY()
		FString hash;
};

USTRUCT()
struct FAddMechaPointData
{
	GENERATED_BODY()

		UPROPERTY()
		int status;
};

USTRUCT()
struct FResponseAddMechaPoint
{
	GENERATED_BODY();

	UPROPERTY()
		FAddMechaPointData AddUserMechaPointResult;
};

USTRUCT()
struct FAddBeastDeathRequest
{
	GENERATED_BODY()

	UPROPERTY()
		FString userid;
	UPROPERTY()
		FString hash;
};

USTRUCT()
struct FAddBeastDeathData
{
	GENERATED_BODY()

	UPROPERTY()
		int status;
};

USTRUCT()
struct FResponseAddBeastDeath
{
	GENERATED_BODY();

	UPROPERTY()
		FAddBeastDeathData AddUserBeastDeathResult;
};

//mecha
USTRUCT()
struct FAddMechaDeathRequest
{
	GENERATED_BODY()

		UPROPERTY()
		FString userid;
	UPROPERTY()
		FString hash;
};

USTRUCT()
struct FAddMechaDeathData
{
	GENERATED_BODY()

		UPROPERTY()
		int status;
};

USTRUCT()
struct FResponseAddMechaDeath
{
	GENERATED_BODY();

	UPROPERTY()
		FAddMechaDeathData AddUserMechaDeathResult;
};
 
UCLASS()
class SHELLS_API AHTTPService : public AActor
{
	GENERATED_BODY()

public:
	AHTTPService();

	void Tick(float DeltaTime) override;

protected:
	void BeginPlay() override;

	FHttpModule* Http;

	FString APIBaseURL;

	TSharedRef<IHttpRequest> RequestWithRoute(FString Subroute);
	TSharedRef<IHttpRequest> GetRequest(FString Subroute);
	TSharedRef<IHttpRequest> PostRequest(FString Subroute, FString ContentJsonString);
	void Send(TSharedRef<IHttpRequest> Request);
	bool ResponseIsValid(FHttpResponsePtr Response, bool WasSuccessful);

	class ACharacter* LocalCharacter;

public:

	UPROPERTY(EditDefaultsOnly, Category = "UI")
		TSubclassOf<class UUserWidget> LoginWidgetClass;

	UPROPERTY(EditAnywhere)
	class UUserWidget* LoginWidget;

	void Login(FLoginRequest LoginCredentials, class APlayerState* playerState);

	void LoginResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, APlayerState* playerState);

	UFUNCTION()
		void OnLoginClicked();

	void AddBeastPoint(FAddBeastPointRequest AddPointInfo);

	void AddBeastPointResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void AddBeastDeath(FAddBeastDeathRequest AddBeastDeathInfo);

	void AddBeastDeathResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	//mecha
	void AddMechaPoint(FAddMechaPointRequest AddMechaPointInfo);

	void AddMechaPointResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void AddMechaDeath(FAddMechaDeathRequest AddMechaDeathInfo);

	void AddMechaDeathResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
