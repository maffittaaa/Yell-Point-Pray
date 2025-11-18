// Fill out your copyright notice in the Description page of Project Settings.


#include "ReadyButton.h"
#include <Players/YPPCustomGameMode.h>
#include <Kismet/GameplayStatics.h>
#include "EnterButton.h"

AReadyButton::AReadyButton()
{
	ButtonAvailable = true;
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;
}

void AReadyButton::Interact_Implementation(AActor* Interactor)
{
    UE_LOG(LogTemp, Warning, TEXT("Ready Button interacted with! :D"));
    UE_LOG(LogTemp, Warning, TEXT("Interactor: %s"), *Interactor->GetClass()->GetName());
    UE_LOG(LogTemp, Warning, TEXT("Owner: %s"), *PlayerReady->GetName());

    if (!ButtonAvailable) return;

    if (Interactor->GetClass() != PlayerReady) return;

    // Call server function to change the state
    ServerToggleReadyState();
}

// Server RPC to handle the state change
void AReadyButton::ServerToggleReadyState_Implementation()
{
    ButtonUsed = !ButtonUsed;
    BigButtonMesh->SetMaterial(0, AlreadyUsed);

    AYPPCustomGameMode* GameMode = Cast<AYPPCustomGameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode)
    {
        if (ButtonUsed) 
        {
            GameMode->PlayerGotReady();
        }
        else 
        {
            GameMode->PlayerCancelReady();
        }

        TArray<AActor*> Buttons;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnterButton::StaticClass(), Buttons);

        AEnterButton* EnterButton = Cast<AEnterButton>(Buttons[0]);

        if (EnterButton)
        {
            if (GameMode->GetPlayersReady() >= 3)
            {
                EnterButton->ChangeAvailability(true);
            }
            else 
            {
                EnterButton->ChangeAvailability(false);
            }
        }
    }

    OnRep_ButtonState();
    // This will automatically trigger OnRep_ButtonState on all clients
    // because ButtonUsed is replicated
}

bool AReadyButton::ServerToggleReadyState_Validate()
{
    return true; // Add validation logic if needed
}