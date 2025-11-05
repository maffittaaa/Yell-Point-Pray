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
		FVector Loc = FoundActor->GetActorLocation();
		UNavigationPath* NavPath = NavSys->FindPathToLocationSynchronously(GetWorld(), Loc, GetActorLocation());
		if (NavPath->GetPathLength() < 200) {
			UE_LOG(LogTemp, Warning, TEXT("Close Enough"));

			Cast<AGuard>(FoundActor)->Called(GetActorLocation());
		}
	}
}

void ARubberDuckPickable::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit){
	if (!HasQuacked) {
		HasQuacked = true;
		CallGuard();
	}
}


