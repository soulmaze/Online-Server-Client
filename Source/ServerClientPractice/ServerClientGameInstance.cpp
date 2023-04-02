// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerClientGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MainMenu.h"
#include "MenuWidget.h"
#include "InGameMenu.h"

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

    UE_LOG(LogTemp, Warning, TEXT("init is working"));

    Subsystem = IOnlineSubsystem::Get();
    if(Subsystem != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Found Subsystem %s"), *Subsystem->GetSubsystemName().ToString());
        SessionInterface = Subsystem->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UServerClientGameInstance::OnCreateSessionComplete);
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
        FOnlineSessionSettings SessionSetting;
        SessionInterface->CreateSession(0, TEXT("My Session"), SessionSetting);
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

void UServerClientGameInstance::Join(FString& Address)
{
    if (Menu != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("teardown is executing"));
        Menu->Teardown();
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
    UE_LOG(LogTemp, Warning, TEXT("LoadMenu triggered"))
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