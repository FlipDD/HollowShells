// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchmakingClient.h"

#include "SocketSubsystem.h"
#include "IPv4Address.h"
#include "IPAddress.h"
#include "RunnableThread.h"

#include "UI/MatchmakingWidget.h"

//MatchmakingClient::MatchmakingClient(UMatchmakingWidget *pWidget)
//{
//	Thread = FRunnableThread::Create(this, TEXT("MatchmakingClientThread"), 0, TPri_Normal);
//	Widget = pWidget;
//}
//
//MatchmakingClient::~MatchmakingClient()
//{
//	bIsRunning = false;
//	delete Thread;
//}
//
//void MatchmakingClient::Stop()
//{
//	bIsRunning = false;
//}
//
//bool MatchmakingClient::IsConnected() const
//{
//	return bIsConnected;
//}
//
//bool MatchmakingClient::Init()
//{
//	const auto SocketInterface = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
//	Socket = SocketInterface->CreateSocket(NAME_Stream, TEXT("default"), false);
//
//	int32 NewSize = 0;
//	Socket->SetReceiveBufferSize(1024, NewSize);
//
//	const auto MatchmakingServerIP = FIPv4Address(127, 0, 0, 1);
//	auto MatchmakingServer = SocketInterface->CreateInternetAddr();
//	MatchmakingServer->SetIp(MatchmakingServerIP.Value);
//	MatchmakingServer->SetPort(8804);
//
//	bIsConnected = Socket->Connect(*MatchmakingServer);
//	if (bIsConnected) {
//		UE_LOG(LogTemp, Log, TEXT("CONNECTED!"));
//		FString Serialized = TEXT("g|#");
//		const auto SerializedChar = Serialized.GetCharArray().GetData();
//
//		const int32 Size = FCString::Strlen(SerializedChar);
//		int32 Sent = 0;
//
//		if (Socket->Send((uint8 *)TCHAR_TO_UTF8(SerializedChar), Size, Sent)) {
//			UE_LOG(LogTemp, Log, TEXT("MESSAGE SENT!"));
//			return true;
//		}
//
//		UE_LOG(LogTemp, Log, TEXT("ERROR: NOT CONNECTED!"));
//		return false;
//	}
//
//	return false;
//}
//
//uint32 MatchmakingClient::Run()
//{
//	bIsRunning = true;
//	TArray<uint8> ReceivedData;
//
//	while (bIsRunning) {
//		uint32 Size = 0;
//
//		if (Socket->HasPendingData(Size)) {
//			int32 Read = 0;
//			ReceivedData.Init(0, Size);
//			Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
//
//			const auto ServerMessage = FString(UTF8_TO_TCHAR(ReceivedData.GetData()));
//			UE_LOG(LogTemp, Log, TEXT("RECEIVED: %s"), *ServerMessage);
//
//			if (ServerMessage[0] == 's' && Widget) {
//				Widget->UpdateSessionsList(ServerMessage);
//			}
//			else if (ServerMessage[0] == 'o' && Widget) {
//				TArray<FString> Out;
//				ServerMessage.ParseIntoArray(Out, TEXT("|"), true);
//				Widget->StartGameHost(FCString::Atoi(*Out[1]));
//			}
//		}
//	}
//	return 0;
//}
//
//void MatchmakingClient::HostNewGame(FString Name, FString Port) const
//{
//	bool bCanBind = false;
//	const auto SocketInterface = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
//	const auto LocalIp = SocketInterface->GetLocalHostAddr(*GLog, bCanBind);
//
//	if (LocalIp->IsValid()) {
//		int32 Sent = 0;
//		const auto Serialized = "h|" + Name + "|" + LocalIp->ToString(false) + "|" + Port + "|#";
//		const auto SerializedChar = Serialized.GetCharArray().GetData();
//		const auto Size = FCString::Strlen(SerializedChar);
//		const auto bSuccessful = Socket->Send((uint8 *)TCHAR_TO_UTF8(SerializedChar), Size, Sent);
//		if (bSuccessful) {
//			UE_LOG(LogTemp, Log, TEXT("MESSAGE SENT!!!!"));
//		}
//	}
//}