// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerClientGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

#include "MainMenu.h"
#include "MenuWidget.h"
#include "InGameMenu.h"

const static FName SESSION_NAME = TEXT("My Session Game");

UServerClientGameInstance::UServerClientGameInstance(const FObjectInitializer & ObjectInitializer)
{
    static ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/MyContents/MenuSystem/Blueprints/WBP_MainMenu"));
    if(!ensure(MenuBPClass.Class != nullptr)) return;
    MenuClass = MenuBPClass.Class;

    static ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/MyContents/InGameMenu/WBP_InGameMenu"));
    if(!ensure(InGameMenuBPClass.Class != nullptr)) return;
    
    InGameMenuClass = InGameMenuBPClass.Class;
}

void UServerClientGameInstance::Init()
{
    Super::Init();

    Subsystem = IOnlineSubsystem::Get();
    if(Subsystem != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Found Subsystem %s"), *Subsystem->GetSubsystemName().ToString());
        SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UServerClientGameInstance::OnCreateSessionComplete);
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this,&UServerClientGameInstance::OnDestroySessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UServerClientGameInstance::OnFindSessionComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UServerClientGameInstance::OnJoinSessionComplete);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Found no subsystem"));
    } 
}

void UServerClientGameInstance::Host()
{
    if (SessionInterface.IsValid())
    {
        auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
        if (ExistingSession != nullptr)
        {
            SessionInterface->DestroySession(SESSION_NAME);
            UE_LOG(LogTemp, Warning, TEXT("the last session has been destroyed"));

        }
        else
        {
            CreateSession();
            UE_LOG(LogTemp, Warning, TEXT("session creation if the SESSION_NAME is null"));
        }
    }
}

void UServerClientGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (!bWasSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("the session creation was not complete"));

        return;
    }
    if (Menu != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("teardown is executing"));
        Menu->Teardown();
    }

    UEngine* Engine = GetEngine();
    if (!ensure(Engine != nullptr)) return;

    Engine->AddOnScreenDebugMessage(0, 2, FColor::Blue, TEXT("Hosting"));

    UWorld* World = GetWorld();
    if (!ensure(World != nullptr)) return;
    World->ServerTravel("/Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap?listen");
}

void UServerClientGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
    UE_LOG(LogTemp, Warning, TEXT("On Destroy Session Complete called"));
    if (bWasSuccessful) 
    {
        CreateSession();
        UE_LOG(LogTemp, Warning, TEXT("session creation after last session distruction"));
    }
}

void UServerClientGameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
    TArray<FOnlineSessionSearchResult> SessionSearchResult = SessionSearch->SearchResults;
    if (bWasSuccessful && SessionSearch.IsValid() && Menu != nullptr)
    {
        TArray<FString> ServerNames;

        for (const FOnlineSessionSearchResult& SearchResult : SessionSearchResult)
        {
            UE_LOG(LogTemp, Warning, TEXT("Session %s has been found"), *SearchResult.GetSessionIdStr());

            ServerNames.Add(SearchResult.GetSessionIdStr());
        }

        Menu->SetServerList(ServerNames);
    }
}


void UServerClientGameInstance::CreateSession()
{
    if (SessionInterface != nullptr)
    {
        FOnlineSessionSettings SessionSetting;
        SessionSetting.bIsLANMatch = true;
        SessionSetting.bShouldAdvertise = true;
        SessionSetting.NumPublicConnections = 2;
        
        SessionInterface->CreateSession(0, SESSION_NAME, SessionSetting);
        UE_LOG(LogTemp, Warning, TEXT("new session has been created without call backing completion"));
    }
}

void UServerClientGameInstance::RefreshServerList()
{
    SessionSearch = MakeShareable(new FOnlineSessionSearch());
    if (SessionSearch.IsValid())
    {
        TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();
        SessionInterface->FindSessions(0, SearchSettingsRef);
        UE_LOG(LogTemp, Warning, TEXT("start finding session"));
    }

}

void UServerClientGameInstance::Join(uint32 Index)
{
    if (!SessionInterface.IsValid()) return;
    if (!SessionSearch.IsValid()) return;

    if (Menu != nullptr)
    {
        Menu->Teardown();
    }
    SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}
void UServerClientGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (!SessionInterface.IsValid()) return;

    FString Address;
    if (!SessionInterface->GetResolvedConnectString(SESSION_NAME, Address))
    {
        UE_LOG(LogTemp, Warning, TEXT("could not resolve connection string"));

        return;
    }

    UEngine* Engine = GetEngine();
    if (!ensure(Engine != nullptr)) return;

    Engine->AddOnScreenDebugMessage(0, 5, FColor::Blue, FString::Printf(TEXT("Joining %s"), *Address));

    APlayerController* PlayerController = GetFirstLocalPlayerController();
    if (!ensure(PlayerController != nullptr)) return;
    PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

void UServerClientGameInstance::InGameLoadMenu()
{
    UE_LOG(LogTemp, Warning, TEXT("InGameLoadMenu triggered"))
    if(!ensure(InGameMenuClass != nullptr)) return;
    UMenuWidget* InGameMenu = CreateWidget<UMenuWidget>(this, InGameMenuClass);//we changed uuserwidget to umainmenu after making menu and interface
    if(!ensure(InGameMenu != nullptr)) return;

    InGameMenu->Setup();

    InGameMenu->SetMenuInterface(this);
}

void UServerClientGameInstance::LoadMenu()
{
    UE_LOG(LogTemp, Warning, TEXT("LoadMenu triggered"));
    if(!ensure(MenuClass != nullptr)) return;
    Menu = CreateWidget<UMainMenu>(this, MenuClass);//we changed uuserwidget to umainmenu after making menu and interface
    if(!ensure(Menu != nullptr)) return;

    Menu->Setup();

    Menu->SetMenuInterface(this);
}

void UServerClientGameInstance::LoadMainMenu()
{
    APlayerController* PlayerController = GetFirstLocalPlayerController();
    if(!ensure(PlayerController != nullptr)) return;
    PlayerController->ClientTravel("/Game/MyContents/MenuSystem/Level/MenuLevel", ETravelType::TRAVEL_Absolute);
}


