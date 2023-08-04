// Fill out your copyright notice in the Description page of Project Settings.

#include "Engine/World.h"
#include "kismet/GameplayStatics.h"


#include "assigment/assigmentgameinstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSetting.h"



Uassigmentgameinstance::Uassigmentgameinstance()
{

}
void Uassigmentgameinstance::Init()
{
	{
		Super::Init();

		if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
		{
			SessionInterface = Subsystem->GetSessionInterface();
			if (SessionInterface.IsValid())
			{
				SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &Uassigmentgameinstance::OnCreateSessionComplete);
				SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &Uassigmentgameinstance::OnFindSessionComplete);
				SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &Uassigmentgameinstance::OnJoinSessionComplete);
			}
		}
	}

	void Uassigmentgameinstance::OnCreateSessionComplete(FName SessionName, bool Succeeded)
	{
		if (Succeeded)
		{
			GetWorld()->ServerTravel("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen");
		}
	}

	void Uassigmentgameinstance::OnFindSessionComplete(bool Succeeded)
	{
		if (Succeeded)
		{
			TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

			if (SearchResults.Num())
			{
				SessionInterface->JoinSession(0, FName("Crete Session"), SearchResults[0]);
			}
		}
	}

	void Uassigmentgameinstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
	{
		if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			FString JoinAddress = "";
			SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
			if (JoinAddress != "")
			{
				PController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
			}
		}
	}

	void Uassigmentgameinstance::CreateServer()
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateServer"));
		FOnlineSessionSettings SessionSettings;
		SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.bIsDedicated = false;
		SessionSettings.bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.NumPublicConnections = 5;

		SessionInterface->CreateSession(0, FName("Crete Session"), SessionSettings);
	}

	void Uassigmentgameinstance::JoinServer()
	{
		SessionSearch = MakeShareable(new FOnlineSessionSearch());
		SessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
		SessionSearch->MaxSearchResults = 10000;
		SessionSearch->QuerySettings.Set("SEARCH_PRESENCE", true, EOnlineComparisonOp::Equals);

		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}