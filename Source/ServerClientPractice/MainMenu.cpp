// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "ServerRow.h"

UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer)
{
    static ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/MyContents/MenuSystem/Blueprints/WBP_ServerRow"));
    if (!ensure(ServerRowBPClass.Class != nullptr)) return;
    ServerRowClass = ServerRowBPClass.Class;
}


bool UMainMenu::Initialize()
{
    bool success = Super:: Initialize();
    if(!success) return false;

    if(!ensure(HostButton != nullptr)) return false;
    HostButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);
    
    if(!ensure(JoinButton != nullptr)) return false;
    JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

    if(!ensure(CancelJoinMenuButton != nullptr)) return false;
    CancelJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

    if(!ensure(ConfirmJoinMenuButton != nullptr)) return false;
    ConfirmJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

    if(!ensure(ExitButton != nullptr)) return false;
    ExitButton->OnClicked.AddDynamic(this, &UMainMenu::ExitPressed);

    return true;
}



void UMainMenu::HostServer()
{
    if(MenuInterface != nullptr)
    {
        MenuInterface->Host();
    }
    
}

void UMainMenu::SetServerList(TArray<FString> ServerNames)
{
    UWorld* World = this->GetWorld();
    if (!ensure(World != nullptr)) return;

    ServerList->ClearChildren();

    uint32 i = 0;

    for (const FString& ServerName : ServerNames)
    {
        UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);
        if (!ensure(Row != nullptr)) return;

        Row->ServerName->SetText(FText::FromString(ServerName));
        Row->Setup(this, i);
        i++;

        ServerList->AddChild(Row);
    }
}

void UMainMenu::SelectIndex(uint32 Index)
{
    SelectedIndex = Index;
}

void UMainMenu::JoinServer()
{
    if (SelectedIndex.IsSet() && MenuInterface != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("selected indesx is %d"), SelectedIndex.GetValue());

        MenuInterface->Join(SelectedIndex.GetValue());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("selected indesx is not set"));
    }
}

void UMainMenu::OpenJoinMenu()
{
    if(!ensure(MenuSwitcher != nullptr)) return;
    if(!ensure(JoinMenu != nullptr)) return;

    MenuSwitcher->SetActiveWidget(JoinMenu);
    if (MenuInterface != nullptr)
    {
        MenuInterface->RefreshServerList();
    }
}

void UMainMenu::OpenMainMenu()
{
    if(!ensure(MenuSwitcher != nullptr)) return;
    if(!ensure(MainMenu != nullptr)) return;

    MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::ExitPressed()
{
    UWorld* World = GetWorld();
    if(!ensure(World != nullptr)) return;

    APlayerController* PlayerController = World->GetFirstPlayerController();
    if(!ensure(PlayerController != nullptr)) return;

    PlayerController->ConsoleCommand("Quit");
}
