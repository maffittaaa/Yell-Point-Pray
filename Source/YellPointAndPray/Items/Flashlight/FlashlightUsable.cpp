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
    
    bReplicates = true;
}

void AFlashlightUsable::Use_Implementation(AActor* User)
{
    bIsLightOn = !bIsLightOn;
    UserStored = Cast<AYellPointAndPrayCharacter>(User);
    
    UserStored->PlayerLight->SetVisibility(bIsLightOn);

    if (GetLocalRole() == ROLE_AutonomousProxy)
    {
        Server_ToggleLight();
    }

    UE_LOG(LogTemp, Warning, TEXT("Flashlight Used CARALHOOO!"));
}

// Add this Server RPC function
void AFlashlightUsable::Server_ToggleLight_Implementation()
{
    bIsLightOn = !bIsLightOn;
    UserStored->PlayerLight->SetVisibility(bIsLightOn);
}

void AFlashlightUsable::OnRep_IsLightOn()
{
    UserStored->PlayerLight->SetVisibility(bIsLightOn);
}

void AFlashlightUsable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AFlashlightUsable, bIsLightOn);
    DOREPLIFETIME(AFlashlightUsable, UserStored);
}