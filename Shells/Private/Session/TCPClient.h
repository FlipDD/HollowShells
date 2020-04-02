// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runnable.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

class SHELLS_API TCPClient : public FRunnable
{
public:
	TCPClient(class AMatchmakingLevelScript *LevelScript);
	~TCPClient();

	//struct FSessionInfo* SessionInfo;

    virtual bool Init() override;
    virtual uint32 Run() override; 
    virtual void Stop() override;

    void HostNewGame(FString Name, FString Port) const;
	bool IsConnected() const;
	void RequestServerList();

	void AddPlayerToSession(int SessionIndex);
	
private:
	FRunnableThread *Thread;
	FSocket *Socket;
	FSocket *ListenerSocket;

    bool bIsRunning;
	bool bIsConnected;
	class AMatchmakingLevelScript *GameLevel;
}; 