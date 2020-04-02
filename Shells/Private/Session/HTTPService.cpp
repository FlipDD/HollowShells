

#include "HTTPService.h"
#include "GameFramework/Character.h"
#include "Explorer/ExplorerCharacter.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "UserWidget.h"
#include "EngineUtils.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "TextBlock.h"
#include "EditableTextBox.h"
#include "MainPlayerState.h"

//AHTTPService::AHTTPService() { APIBaseURL = TEXT("http://localhost:5001/api"); }
AHTTPService::AHTTPService() { APIBaseURL = TEXT("http://localhost:63818/Service1.svc"); }

void AHTTPService::BeginPlay() {
	Super::BeginPlay();
	Http = &FHttpModule::Get();

	if (LoginWidgetClass) {
		LoginWidget = CreateWidget<UUserWidget>(GetWorld(), LoginWidgetClass);
		LoginWidget->AddToViewport();

		UButton* LoginButton = Cast<UButton>(LoginWidget->GetWidgetFromName(TEXT("LoginButton")));
		if (LoginButton)
		{
			UE_LOG(LogTemp, Warning, TEXT("Created button"));

			LoginButton->OnClicked.AddDynamic(this, &AHTTPService::OnLoginClicked);
		}


		for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
			auto Character = Cast<ACharacter>(*ActorItr);
			if (Character && Character->IsLocallyControlled()) {
				LocalCharacter = Character;
				LocalCharacter->DisableInput(nullptr);
				auto Controller = Cast<APlayerController>(LocalCharacter->GetController());
				if (Controller)
				{
					Controller->bShowMouseCursor = true;
					Controller->bEnableClickEvents = true;
					Controller->bEnableMouseOverEvents = true;
				}
			}
		}
	}
}

TSharedRef<IHttpRequest> AHTTPService::RequestWithRoute(FString Subroute) {
	auto Request = Http->CreateRequest();
	Request->SetURL(APIBaseURL + Subroute);
	Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
	return Request;
}

TSharedRef<IHttpRequest> AHTTPService::GetRequest(FString Subroute) {
	auto Request = RequestWithRoute(Subroute);
	Request->SetVerb("GET");
	return Request;
}

TSharedRef<IHttpRequest> AHTTPService::PostRequest(FString Subroute, FString ContentJsonString) {
	auto Request = RequestWithRoute(Subroute);
	Request->SetVerb("POST");
	Request->SetContentAsString(ContentJsonString);
	return Request;
}

void AHTTPService::Send(TSharedRef<IHttpRequest> Request) {
	Request->ProcessRequest();
}

bool AHTTPService::ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessfull) {
	UE_LOG(LogTemp, Warning, TEXT("Hope no error %d"), Response->GetResponseCode());

	
	if (!bWasSuccessfull || !Response.IsValid())
		return false;

	if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
		return true;

	UE_LOG(LogTemp, Warning, TEXT("HTTP ERROR: %d"), Response->GetResponseCode());

	return false;
}

void AHTTPService::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AHTTPService::Login(FLoginRequest LoginCredentials, APlayerState* playerState)
{
	FString ContentJsonString;
	FJsonObjectConverter::UStructToJsonObjectString(
		FLoginRequest::StaticStruct(),
		&LoginCredentials,
		ContentJsonString, 0, 0);

	UE_LOG(LogTemp, Warning, (TEXT("Content is: %s")), *ContentJsonString);

	TSharedRef<IHttpRequest> Request = PostRequest("/login", ContentJsonString);
	Request->OnProcessRequestComplete().BindUObject(
		this, &AHTTPService::LoginResponse, playerState);
	Send(Request);
}

void AHTTPService::LoginResponse(FHttpRequestPtr Request,
	FHttpResponsePtr Response, bool bWasSuccessful, class APlayerState* playerState)
{
	if (!ResponseIsValid(Response, bWasSuccessful))
		return;

	FString JsonString = Response->GetContentAsString();

	FLoginResult LoginResponse;
	FJsonObjectConverter::JsonObjectStringToUStruct<FLoginResult>(
		JsonString, &LoginResponse, 0, 0);

	if (LoginResponse.LoginResult.status == 0) {
		LoginWidget->RemoveFromViewport();
		LocalCharacter->EnableInput(nullptr);
		auto PlayerController = Cast<APlayerController>(LocalCharacter->GetController());
		PlayerController->bShowMouseCursor = false;
		PlayerController->bEnableClickEvents = false;
		PlayerController->bEnableMouseOverEvents = false;

		auto pState = Cast<AMainPlayerState>(playerState);
		pState->InitPlayerSession(this, LoginResponse.LoginResult.hash,
			LoginResponse.LoginResult.userid,
			LoginResponse.LoginResult.name,
			LoginResponse.LoginResult.totalbeastpoints,
			LoginResponse.LoginResult.totalbeastdeaths,
			LoginResponse.LoginResult.totalmechapoints,
			LoginResponse.LoginResult.totalmechadeaths);

	}
	else {
		auto ErrorText = Cast<UTextBlock>(LoginWidget->GetWidgetFromName(TEXT("ErrorMessage")));
		if (ErrorText)
			ErrorText->SetVisibility(ESlateVisibility::Visible);
	}
}

void AHTTPService::OnLoginClicked()
{
	auto EmailText = Cast<UEditableTextBox>(LoginWidget->GetWidgetFromName(TEXT("EmailTextBox")));
	auto PasswordText = Cast<UEditableTextBox>(LoginWidget->GetWidgetFromName(TEXT("PasswordTextBox")));
	auto ErrorText = Cast<UTextBlock>(LoginWidget->GetWidgetFromName(TEXT("ErrorMessage")));

	if (EmailText && PasswordText && ErrorText)
	{
		FLoginRequest LoginCredentials;
		LoginCredentials.email = EmailText->GetText().ToString();
		LoginCredentials.password = PasswordText->GetText().ToString();
		ErrorText->SetVisibility(ESlateVisibility::Hidden);

		Login(LoginCredentials, LocalCharacter->GetPlayerState());
	}
}

void AHTTPService::AddBeastPoint(FAddBeastPointRequest AddBeastPointInfo)
{
	FString ContentJsonString;
	FJsonObjectConverter::UStructToJsonObjectString(FAddBeastPointRequest::
		StaticStruct(), &AddBeastPointInfo, ContentJsonString, 0, 0);
	TSharedRef<IHttpRequest> Request = PostRequest("/AddBeastPoint",
		ContentJsonString);
	Request->OnProcessRequestComplete().BindUObject(this,
		&AHTTPService::AddBeastPointResponse);
	Send(Request);

}

void AHTTPService::AddBeastPointResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!ResponseIsValid(Response, bWasSuccessful)) {
		return;
	}
	FResponseAddBeastPoint AddBeastPointResponse;
	FString JsonString = Response->GetContentAsString();
	FJsonObjectConverter::JsonObjectStringToUStruct<FResponseAddBeastPoint>(
		JsonString, &AddBeastPointResponse, 0, 0);
	if (AddBeastPointResponse.AddUserBeastPointResult.status == 0) {
		UE_LOG(LogTemp, Warning, TEXT("AddPoint Succeeded!"));
	}
}

void AHTTPService::AddBeastDeath(FAddBeastDeathRequest AddDeathInfo)
{
	FString ContentJsonString;
	FJsonObjectConverter::UStructToJsonObjectString(FAddBeastPointRequest::
		StaticStruct(), &AddDeathInfo, ContentJsonString, 0, 0);
	TSharedRef<IHttpRequest> Request = PostRequest("/AddBeastDeath",
		ContentJsonString);
	Request->OnProcessRequestComplete().BindUObject(this,
		&AHTTPService::AddBeastDeathResponse);
	Send(Request);

}

void AHTTPService::AddBeastDeathResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!ResponseIsValid(Response, bWasSuccessful)) {
		return;
	}
	FResponseAddBeastDeath AddBeastDeathResponse;
	FString JsonString = Response->GetContentAsString();
	FJsonObjectConverter::JsonObjectStringToUStruct<FResponseAddBeastDeath>(
		JsonString, &AddBeastDeathResponse, 0, 0);
	if (AddBeastDeathResponse.AddUserBeastDeathResult.status == 0) {
		UE_LOG(LogTemp, Warning, TEXT("AddDeath Succeeded!"));
	}
}

//Mecha
void AHTTPService::AddMechaPoint(FAddMechaPointRequest AddMechaPointInfo)
{
	FString ContentJsonString;
	FJsonObjectConverter::UStructToJsonObjectString(FAddMechaPointRequest::
		StaticStruct(), &AddMechaPointInfo, ContentJsonString, 0, 0);
	TSharedRef<IHttpRequest> Request = PostRequest("/AddMechaPoint",
		ContentJsonString);
	Request->OnProcessRequestComplete().BindUObject(this,
		&AHTTPService::AddMechaPointResponse);
	Send(Request);

}

void AHTTPService::AddMechaPointResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!ResponseIsValid(Response, bWasSuccessful)) {
		return;
	}
	FResponseAddMechaPoint AddMechaPointResponse;
	FString JsonString = Response->GetContentAsString();
	FJsonObjectConverter::JsonObjectStringToUStruct<FResponseAddMechaPoint>(
		JsonString, &AddMechaPointResponse, 0, 0);
	if (AddMechaPointResponse.AddUserMechaPointResult.status == 0) {
		UE_LOG(LogTemp, Warning, TEXT("AddPoint Succeeded!"));
	}
}

void AHTTPService::AddMechaDeath(FAddMechaDeathRequest AddMechaDeathInfo)
{
	FString ContentJsonString;
	FJsonObjectConverter::UStructToJsonObjectString(FAddMechaDeathRequest::
		StaticStruct(), &AddMechaDeathInfo, ContentJsonString, 0, 0);
	TSharedRef<IHttpRequest> Request = PostRequest("/AddMechaDeath",
		ContentJsonString);
	Request->OnProcessRequestComplete().BindUObject(this,
		&AHTTPService::AddMechaDeathResponse);
	Send(Request);

}

void AHTTPService::AddMechaDeathResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!ResponseIsValid(Response, bWasSuccessful)) {
		return;
	}
	FResponseAddMechaDeath AddMechaDeathResponse;
	FString JsonString = Response->GetContentAsString();
	FJsonObjectConverter::JsonObjectStringToUStruct<FResponseAddMechaDeath>(
		JsonString, &AddMechaDeathResponse, 0, 0);
	if (AddMechaDeathResponse.AddUserMechaDeathResult.status == 0) {
		UE_LOG(LogTemp, Warning, TEXT("AddDeath Succeeded!"));
	}
}
