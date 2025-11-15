// Fill out your copyright notice in the Description page of Project Settings.

#include "EnterButton.h"
#include <Players/YPPCustomGameMode.h>

void AEnterButton::Interact_Implementation(AActor* Interactor)
{
    UE_LOG(LogTemp, Warning, TEXT("Enter Button interacted with! :D"));

    if (!ButtonAvailable) return;
    
    if (ButtonUsed) return;
    ServerToggleEnterState();
}

void AEnterButton::ServerToggleEnterState_Implementation()
{
    ButtonUsed = true;
    OnRep_ButtonState();

    AYPPCustomGameMode* GameMode = Cast<AYPPCustomGameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode)
    {
        GameMode->EnterGame();
    }
}

bool AEnterButton::ServerToggleEnterState_Validate()
{
    return true;
}

void AEnterButton::ChangeAvailability_Implementation(bool State)
{
    ButtonAvailable = State;

    if (ButtonAvailable)
    {
        BigButtonMesh->SetMaterial(0, PreparedToBeUsed);
    }
    else
    {
        BigButtonMesh->SetMaterial(0, CantBeUsed);
    }
}