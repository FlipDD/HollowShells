
#include "TCPClient.h"
#include "SocketSubsystem.h"
#include "IPv4Address.h"
#include "IPAddress.h"
#include "RunnableThread.h"
#include "MatchmakingLevelScript.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Sockets.h"


TCPClient::TCPClient(AMatchmakingLevelScript *LevelScript)
{
	Thread = FRunnableThread::Create(this, TEXT("TCPClientThread"), 0, TPri_Normal);
	GameLevel = LevelScript;
}

TCPClient::~TCPClient()
{
	Stop();
	
	//delete Thread;
}

void TCPClient::Stop()
{
	bIsRunning = false;
}

bool TCPClient::IsConnected() const
{
    return bIsConnected;
}

bool TCPClient::Init()
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->
		CreateSocket(NAME_Stream, TEXT("Default"), false);
	int NewSize = 0;
	Socket->SetReceiveBufferSize(1024, NewSize);
	//put your own ip address
	FIPv4Address MatchMakingServerIP(127, 0, 0, 1);

	TSharedRef<FInternetAddr> MatchMakingServer = ISocketSubsystem::
		Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	MatchMakingServer->SetIp(MatchMakingServerIP.Value);
	MatchMakingServer->SetPort(8804);

	bIsConnected = Socket->Connect(*MatchMakingServer);

	if (bIsConnected)
	{
		UE_LOG(LogTemp, Warning, TEXT("CONNECTED!!"));
		FString message = TEXT("g|#");
		TCHAR* convertedMessage = message.GetCharArray().GetData();
		int32 size = FCString::Strlen(convertedMessage);
		int32 sent = 0;
		bool success = Socket->Send((uint8*)TCHAR_TO_UTF8(convertedMessage), size, sent);
		if (success)
		{
			UE_LOG(LogTemp, Warning, TEXT("Message sent!!"));
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Message not sent!!"));
			return false;
		}

	}
	return false;
}

uint32 TCPClient::Run()
{
	bIsRunning = true;
	TArray<uint8> ReceivedData;
	while (bIsRunning)
	{
		uint32 size = 0;
		if (Socket->HasPendingData(size))
		{
			ReceivedData.Init(0, size);
			int32 readsize = 0;
			Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), readsize);
			FString ServerMessage = FString(UTF8_TO_TCHAR(ReceivedData.GetData()));

			UE_LOG(LogTemp, Warning, TEXT("Recieved: %s"), *ServerMessage);
			if (ServerMessage[0] == 's')
			{
				// MAdded - Probably wrong lol
				GameLevel->UpdateSessionsList(ServerMessage);
			}
			else if (ServerMessage[0] == 'o')
			{
				bIsRunning = false;
				TArray<FString> Out;
				ServerMessage.ParseIntoArray(Out, TEXT("|"), true);
				GameLevel->StartGameHost(FCString::Atoi(*Out[1]), 0);
			}
			else if (ServerMessage[0] == 'k')
			{
				TArray<FString> Out;
				ServerMessage.ParseIntoArray(Out, TEXT("|"), true);
				//GameLevel->AddPlayer(FCString::Atoi(*Out[1]));
			}
		}
	}
	return 0;
}

void TCPClient::HostNewGame(FString Name, FString Port) const
{
	bool bCanBind = false;
	TSharedRef<FInternetAddr> localIp = ISocketSubsystem::Get(
		PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBind);

	if (localIp->IsValid()) {
		FString serialized = "h|" + Name + "|" + localIp->
			ToString(false) + "|" + Port + "|#";
		TCHAR* serializedChar = serialized.GetCharArray().GetData();
		int32 size = FCString::Strlen(serializedChar);
		int32 sent = 0;

		bool successful = Socket->Send((uint8*)TCHAR_TO_UTF8(serializedChar), size, sent);
		if (successful)
		{
			// MAdded player

			UE_LOG(LogTemp, Warning, TEXT("MESSAGE SENT!!!!"));
		}
	}
}

void TCPClient::AddPlayerToSession(int SessionIndex)
{
	FString index = FString::FromInt(SessionIndex);
	FString message = TEXT("j|" + index + "|#");
	TCHAR* ConvertedData = message.GetCharArray().GetData();
	int32 size = FCString::Strlen(ConvertedData);
	int32 sent = 0;

	bool successful = Socket->Send((uint8*)TCHAR_TO_UTF8(ConvertedData), size, sent);
	if (successful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Added new player"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Adding new player wasnt successful"));
	}
}

void TCPClient::RequestServerList()
{
	FString message = TEXT("g|#");
	TCHAR* ConvertedData = message.GetCharArray().GetData();
	int32 size = FCString::Strlen(ConvertedData);
	int32 sent = 0;

	bool successful = Socket->Send((uint8*)TCHAR_TO_UTF8(ConvertedData), size, sent);
	if (successful)
	{
		UE_LOG(LogTemp, Warning, TEXT("REQUEST SERVER SENT!!!!"));
	}
}
