// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"

void UMenuWidget::Setup()
{
     this->AddToViewport();

    UWorld* World = GetWorld();
    if(!ensure(World != nullptr)) return;

    APlayerController* PlayerController = World->GetFirstPlayerController();
    if(!ensure(PlayerController != nullptr)) return;

    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(this->TakeWidget());
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    
    PlayerController->SetInputMode(InputMode);

    PlayerController->bShowMouseCursor = true;
}

void UMenuWidget::Teardown()
{
    this->RemoveFromViewport();

    UWorld* World = GetWorld();
    if(!ensure(World != nullptr)) return;

    APlayerController* PlayerController = World->GetFirstPlayerController();
    if(!ensure(PlayerController != nullptr)) return;

    FInputModeGameOnly InputMode;
    PlayerController->SetInputMode(InputMode);

    PlayerController->bShowMouseCursor = false;
}

void UMenuWidget::SetMenuInterface(IMenuInterface* MInterface)
{
    this->MenuInterface = MInterface;
}