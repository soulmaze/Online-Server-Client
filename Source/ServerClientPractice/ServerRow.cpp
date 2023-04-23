// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerRow.h"
#include "Components/Button.h"

void UServerRow::Setup(class UMainMenu* InParent, uint32 InIndex)
{
    Parent = InParent;
    Index = InIndex;

    RowButton->OnClicked.AddDynamic(this, &UServerRow::OnClickedRowButton);
}

void UServerRow::OnClickedRowButton()
{
    Parent->SelectIndex(Index);
}
