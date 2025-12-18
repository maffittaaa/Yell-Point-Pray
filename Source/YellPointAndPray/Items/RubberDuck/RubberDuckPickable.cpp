// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/RubberDuck/RubberDuckPickable.h"
#include <Net/UnrealNetwork.h>
#include <Kismet/GameplayStatics.h>
#include "Obstacles/Guard/Guard.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Obstacles/Guard/Guard.h"

ARubberDuckPickable::ARubberDuckPickable() {
	Name = "Rubber Duck Pickable";
	ID = 4;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereCollider->InitSphereRadius(50.0f);
	SphereCollider->SetupAttachment(Mesh);

	DuckAudioPlayer = CreateDefaultSubobject<UAudioComponent>(TEXT("DuckAudioPlayer"));
	DuckAudioPlayer->SetupAttachment(Mesh);
}

void ARubberDuckPickable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARubberDuckPickable, HasQuacked);

}

void ARubberDuckPickable::BeginPlay()
{
	Super::BeginPlay();

	if (SphereCollider) {
		Mesh->OnComponentHit.AddDynamic(this, &ARubberDuckPickable::OnHit);
	}
}

void ARubberDuckPickable::CallGuard() {
	UE_LOG(LogTemp, Warning, TEXT("Quack Quack Bitch"));

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGuard::StaticClass(), FoundActors);
	for (auto& FoundActor : FoundActors)
	{
		if (!Cast<AGuard>(FoundActor))continue;
			

		FVector Loc = FoundActor->GetActorTransform().GetLocation();

//		UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), Loc.X);

		FVector Loc2 = Mesh->GetComponentLocation();
		
		UNavigationPath* NavPath = NavSys->FindPathToLocationSynchronously(GetWorld(), Loc, Loc2);
		float da = NavPath->GetPathLength();
		UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), da);
		if (da < 4000) {
			UE_LOG(LogTemp, Warning, TEXT("Close Enough"));	

			FVector duckLoc = Loc2;
			Cast<AGuard>(FoundActor)->Called(duckLoc);
		}
	}
}

void ARubberDuckPickable::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit){
	if (!HasQuacked && HasAuthority()) {
		HasQuacked = true;
		CallGuard();
	}
	
	//START QUACK QUACK
	int32 RandSound = FMath::RandRange(0, 3);
	UE_LOG(LogTemp, Warning, TEXT("RandSound: %d"), RandSound);

	if (!DuckAudioPlayer->IsPlaying())
	{
		DuckAudioPlayer->Sound = QuackSoundsList[RandSound];
		DuckAudioPlayer->Play();
	}
	//END QUACK QUACK SOUND
}


