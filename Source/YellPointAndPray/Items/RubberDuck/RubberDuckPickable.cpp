// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/RubberDuck/RubberDuckPickable.h"
#include <Net/UnrealNetwork.h>

ARubberDuckPickable::ARubberDuckPickable() {
	Name = "Rubber Duck";
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
}

void ARubberDuckPickable::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit){
	if (!HasQuacked) {
		HasQuacked = true;
		CallGuard();
	}
}


