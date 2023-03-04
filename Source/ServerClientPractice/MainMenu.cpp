// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"

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

void UMainMenu::JoinServer()
{
    if(MenuInterface != nullptr)
    {
        if(!ensure(IpAddressField != nullptr)) return;
        FString Address = IpAddressField->GetText().ToString();
        MenuInterface->Join(Address);
    }
}

void UMainMenu::OpenJoinMenu()
{
    if(!ensure(MenuSwitcher != nullptr)) return;
    if(!ensure(JoinMenu != nullptr)) return;

    MenuSwitcher->SetActiveWidget(JoinMenu);
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
