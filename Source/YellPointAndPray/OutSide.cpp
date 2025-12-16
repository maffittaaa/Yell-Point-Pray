// Fill out your copyright notice in the Description page of Project Settings.


#include "OutSide.h"
#include "YellPointAndPrayCharacter.h"

// Sets default values
AOutSide::AOutSide()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("SphereCollider"));
	BoxCollider->SetupAttachment(RootComponent);

	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AOutSide::OnOverlapBegin);
	BoxCollider->OnComponentEndOverlap.AddDynamic(this, &AOutSide::OnOverlapEnd);

}


void AOutSide::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	AYellPointAndPrayCharacter* player = Cast<AYellPointAndPrayCharacter>(OtherActor);
	if (player) {
		player->IsOutSide = true;
	}
}

void AOutSide::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	AYellPointAndPrayCharacter* player = Cast<AYellPointAndPrayCharacter>(OtherActor);
	if (player) {
		player->IsOutSide = false;
	}
}
