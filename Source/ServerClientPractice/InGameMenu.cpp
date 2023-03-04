// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenu.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

bool UInGameMenu::Initialize()
{
    bool success = Super:: Initialize();
    if(!success) return false;

    if(!ensure(CancelButton != nullptr)) return false;
    CancelButton->OnClicked.AddDynamic(this, &UInGameMenu::CancelPressed);
    if(!ensure(QuitButton != nullptr)) return false;
    QuitButton->OnClicked.AddDynamic(this, &UInGameMenu::QuitPressed);

    return true;
}

void UInGameMenu::CancelPressed()
{
    Teardown();
}

void UInGameMenu::QuitPressed()
{
    if(MenuInterface != nullptr)
    {
        Teardown();
        MenuInterface->LoadMainMenu();
    }
}