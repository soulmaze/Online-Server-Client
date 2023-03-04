// Fill out your copyright notice in the Description page of Project Settings.

#include "MovingPlatform.h"

AMovingPlatform::AMovingPlatform()
{
    PrimaryActorTick.bCanEverTick = true;
    SetMobility(EComponentMobility::Movable);
}
void AMovingPlatform::BeginPlay()
{
    Super::BeginPlay();

    if(HasAuthority())
    {
        SetReplicates(true);
        SetReplicateMovement(true);
    }

    GlobalStartLocation = GetActorLocation();
    GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);
}

void AMovingPlatform::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if(ActiveTrigger > 0)
    {
        if(HasAuthority())
        {
            FVector Location =  GetActorLocation();
            float JourneyLength = (GlobalTargetLocation - GlobalStartLocation).Size();
            float JourneyTravelled = (Location - GlobalStartLocation).Size();
            
            if(JourneyLength <= JourneyTravelled)
            {
                FVector Temp = GlobalStartLocation;
                GlobalStartLocation = GlobalTargetLocation;
                GlobalTargetLocation = Temp;
            }

            FVector Direction = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal();
            Location +=Speed * DeltaTime * Direction;
            SetActorLocation(Location); 
        }
    }
}

void AMovingPlatform::AddActiveTrigger()
{
    ActiveTrigger++;
	UE_LOG(LogTemp, Warning, TEXT("%d"), ActiveTrigger)
}

void AMovingPlatform::RemoveActiveTrigger()
{
    if(ActiveTrigger > 0)
    {
        ActiveTrigger--;
	UE_LOG(LogTemp, Warning, TEXT("%d"), ActiveTrigger)
    }
}