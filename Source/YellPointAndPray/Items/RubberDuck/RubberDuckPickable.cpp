// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/RubberDuck/RubberDuckPickable.h"

ARubberDuckPickable::ARubberDuckPickable() {
	Name = "Rubber Duck";
	ID = 4;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereCollider->InitSphereRadius(50.0f);
	SphereCollider->SetupAttachment(Mesh);
}

void ARubberDuckPickable::BeginPlay()
{
	Super::BeginPlay();

	if (SphereCollider) {
		SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &ARubberDuckPickable::OnOverlapBegin);
	}
}

void ARubberDuckPickable::CallGuard() {
	UE_LOG(LogTemp, Warning, TEXT("Quack Quack Bitch"));
}

void ARubberDuckPickable::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if (!HasQuacked) {
		HasQuacked = true;
		CallGuard();
	}
}


