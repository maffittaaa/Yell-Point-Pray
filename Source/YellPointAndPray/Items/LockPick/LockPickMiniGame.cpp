// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/LockPick/LockPickMiniGame.h"
#include <YellPointAndPrayCharacter.h>
#include "Door.h"
#include "YellPointAndPrayPlayerController.h"

ALockPickMiniGame::ALockPickMiniGame()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

void ALockPickMiniGame::BeginPlay() {
	Super::BeginPlay();

	GameOverWidget->AddToViewport();

	Arrow = GameOverWidget->GetWidgetFromName(TEXT("Arrow"));
	HitSpot = GameOverWidget->GetWidgetFromName(TEXT("HitSpot"));
	Bar = GameOverWidget->GetWidgetFromName(TEXT("Bar"));
	ArrowSlot = Cast<UCanvasPanelSlot>(Arrow->Slot);
	BarSlot = Cast<UCanvasPanelSlot>(Bar->Slot);
	HitSpotSlot = Cast<UCanvasPanelSlot>(HitSpot->Slot);

	float up = BarSlot->GetPosition().Y + (BarSlot->GetSize().Y / 2);
	float down = BarSlot->GetPosition().Y - (BarSlot->GetSize().Y / 2);

	BarTop = BarSlot->GetPosition().Y + (BarSlot->GetSize().Y / 2);
	BarBottom = BarSlot->GetPosition().Y - (BarSlot->GetSize().Y / 2);

	float hitPos = FMath::FRandRange(down + (HitSpotSlot->GetSize().Y/2), up - (HitSpotSlot->GetSize().Y / 2));
	FVector2D pos = FVector2D(HitSpotSlot->GetPosition().X, hitPos);
	HitSpotSlot->SetPosition(pos);
}

void ALockPickMiniGame::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!ArrowSlot)
        return;

    FVector2D Pos = ArrowSlot->GetPosition();

    float MoveAmount = ArrowSpeed * DeltaTime;

    if (bGoingDown)
    {
        Pos.Y += MoveAmount;

        if (Pos.Y >= BarTop)
        {
            Pos.Y = BarTop;
            bGoingDown = false;
        }
    }
    else
    {
        Pos.Y -= MoveAmount;

        if (Pos.Y <= BarBottom)
        {
            Pos.Y = BarBottom;
            bGoingDown = true;
        }
    }

    ArrowSlot->SetPosition(Pos);
}

void ALockPickMiniGame::OnClick(AActor* User1)
{
    GameOverWidget->RemoveFromParent();

    User = User1;

    float arrowY = ArrowSlot->GetPosition().Y;
    float max = HitSpotSlot->GetPosition().Y - (HitSpotSlot->GetSize().Y / 2);
    float min = HitSpotSlot->GetPosition().Y + (HitSpotSlot->GetSize().Y / 2);

    if (arrowY < min && arrowY > max) {
        Cast<AYellPointAndPrayPlayerController>(Owner)->Unlock(DoorOpening);
        int rand = FMath::FRandRange(1.f, 5.f);
        if (rand == 1) {
            Cast<AYellPointAndPrayCharacter>(User)->InventoryComponent->DeleteInventorySlot(Cast<AYellPointAndPrayCharacter>(User)->InventoryComponent->CurrentItemSelected);
            Cast<AYellPointAndPrayCharacter>(User)->ServerDeleteItem();
        }
    }
    else {
        Cast<AYellPointAndPrayCharacter>(User)->InventoryComponent->DeleteInventorySlot(Cast<AYellPointAndPrayCharacter>(User)->InventoryComponent->CurrentItemSelected);
        Cast<AYellPointAndPrayCharacter>(User)->ServerDeleteItem();

    }
    GetWorldTimerManager().SetTimer(
        DelayHandle,
        this,
        &ALockPickMiniGame::EnableControls,
        0.1f,
        false
    );
}

void ALockPickMiniGame::EnableControls() {
    Cast<AYellPointAndPrayCharacter>(User)->SetMovementInputEnabled(true);
    Cast<AYellPointAndPrayCharacter>(User)->SetLookInputEnabled(true);
    Cast<AYellPointAndPrayCharacter>(User)->SetUseActive(true);

    Destroy();
}
