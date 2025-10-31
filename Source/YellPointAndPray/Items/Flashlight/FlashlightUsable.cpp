// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Flashlight/FlashlightUsable.h"
#include <Net/UnrealNetwork.h>

AFlashlightUsable::AFlashlightUsable()
{
    Name = "Flashlight Usable";
    ID = 5;

    bIsLightOn = false;

    Light = CreateDefaultSubobject<USpotLightComponent>(TEXT("Light"));
    Light->SetupAttachment(Mesh);
    Light->SetVisibility(bIsLightOn);

    SetReplicates(true);
    bReplicates = true;
}

void AFlashlightUsable::Use_Implementation(AActor* User)
{
    UE_LOG(LogTemp, Warning, TEXT("Flashlight Use called - Local Role: %d, Owner: %s, NetMode: %d"),
        (int)GetLocalRole(), *GetNameSafe(GetOwner()), (int)GetNetMode());

    // Always toggle locally for immediate feedback
    bIsLightOn = !bIsLightOn;
    Light->SetVisibility(bIsLightOn);

    // If we're a client, call server RPC
    if (GetLocalRole() == ROLE_AutonomousProxy)
    {
        UE_LOG(LogTemp, Warning, TEXT("Client calling Server_ToggleLight"));
        Server_ToggleLight();
    }
    // If we're the server, replication will handle it automatically
    else if (HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("Server updating light directly"));
    }
}

// Add this Server RPC function
void AFlashlightUsable::Server_ToggleLight_Implementation()
{
    UE_LOG(LogTemp, Warning, TEXT("Server_ToggleLight_Implementation called on server!"));

    // Server updates the state
    bIsLightOn = !bIsLightOn;
    Light->SetVisibility(bIsLightOn);
    UE_LOG(LogTemp, Warning, TEXT("Server processed toggle - Light: %d"), bIsLightOn);
}

void AFlashlightUsable::OnRep_IsLightOn()
{
    UE_LOG(LogTemp, Warning, TEXT("OnRep_IsLightOn - Light state replicated: %d"), bIsLightOn);
    Light->SetVisibility(bIsLightOn);
}

void AFlashlightUsable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // Fix the DOREPLIFETIME macro usage
    //DOREPLIFETIME(AFlashlightUsable, Light);
    DOREPLIFETIME(AFlashlightUsable, bIsLightOn);
}