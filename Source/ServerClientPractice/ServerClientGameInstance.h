// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuInterface.h"
#include "OnlineSubsystem.h"
#include "ServerClientGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SERVERCLIENTPRACTICE_API UServerClientGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

public:

	UServerClientGameInstance(const FObjectInitializer & ObjectInitializer);

	virtual void Init() override;

	UFUNCTION(Exec)
	void Host() override;

	UFUNCTION(Exec)
	void Join(FString& Address) override;

	UFUNCTION(BlueprintCallable)
	void LoadMenu();

	UFUNCTION(BlueprintCallable)
	void InGameLoadMenu();

	void LoadMainMenu() override;

private:

	TSubclassOf<class UUserWidget> MenuClass;
	
	TSubclassOf<class UUserWidget> InGameMenuClass;

	class UMainMenu* Menu;

	IOnlineSubsystem* Subsystem;
};
