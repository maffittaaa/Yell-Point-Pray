#include "Obstacles/Laser/Laser.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ALaser::ALaser() {
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	bAlwaysRelevant = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	collisionSphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	collisionSphere->SetupAttachment(RootComponent);
	collisionSphere->SetSphereRadius(100.0f);
	collisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	laserMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickUpMesh"));
	laserMesh->SetupAttachment(RootComponent);
	laserMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	niagaraLaser = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NSLaser"));
	niagaraLaser->SetupAttachment(RootComponent);

	niagaraLaserImpact = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NSLaserImpact"));
	niagaraLaserImpact->SetupAttachment(RootComponent);
}

void ALaser::BeginPlay() {
	Super::BeginPlay();
	SetLaserColors();
}

void ALaser::Reset_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Laser-specific reset called!"));
}

void ALaser::SetLaserColors() {
	if (niagaraLaser)
		niagaraLaser->SetVariableLinearColor(FName("User.BeamColor"), FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));

	if (niagaraLaserImpact)
		niagaraLaserImpact->SetVariableLinearColor(FName("User.Colour"), FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
}

void ALaser::TouchingLaser(AYellPointAndPrayCharacter* character) {
	UE_LOG(LogTemp, Warning, TEXT("Oh shit"));
	UNavigationSystemV1* navigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGuard::StaticClass(), FoundActors);
	for (auto& FoundActor : FoundActors)
	{
		if (!Cast<AGuard>(FoundActor))
			break;
		
		FVector location = FoundActor->GetActorTransform().GetLocation();
		FVector location2 = laserMesh->GetComponentLocation();
		
		UNavigationPath* navMeshPath = navigationSystem->FindPathToLocationSynchronously(GetWorld(), location, location2);
		float length = navMeshPath->GetPathLength();
		UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), length);
		if (length < 4000) {
			UE_LOG(LogTemp, Warning, TEXT("Close Enough"));	
			Cast<AGuard>(FoundActor)->Called(character->GetActorTransform().GetLocation());
		}
	}
}

void ALaser::SetLaserActive(bool bActive)
{
	if (bIsLaserActive == bActive)
		return;
	
	if (HasAuthority()) {
		bIsLaserActive = bActive;
		OnRep_IsLaserActive();
	}
}

void ALaser::UpdateLaserVisuals()
{
	if (niagaraLaser) {
		niagaraLaser->ResetSystem();
		if (bIsLaserActive) {
			niagaraLaser->Activate();
			niagaraLaser->SetVisibility(true);
		} else {
			niagaraLaser->Deactivate();
			niagaraLaser->SetVisibility(false);
		}
	}
    
	if (niagaraLaserImpact) {
		niagaraLaserImpact->ResetSystem();
		if (bIsLaserActive) {
			niagaraLaserImpact->Activate();
			niagaraLaserImpact->SetVisibility(true);
		} else {
			niagaraLaserImpact->Deactivate();
			niagaraLaserImpact->SetVisibility(false);
		}
	}
    
	if (collisionSphere)
		collisionSphere->SetCollisionEnabled(bIsLaserActive ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
    
	SetActorTickEnabled(bIsLaserActive);
}

void ALaser::DeactivateLaserTemporarily(float duration) {

	if (!HasAuthority()) {
		Server_DeactivateLaserTemporarily(duration);
		return;
	}
	
	if (!bIsLaserActive)
		return;

	Multicast_DeactivateLaserTemporarily(duration);

	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &ALaser::ReactivateLasers);

	SetLaserActive(false);

	GetWorld()->GetTimerManager().SetTimer(
		reactivateTimerHandle,
		Delegate,
		duration,
		false
	);
}

void ALaser::Server_DeactivateLaserTemporarily_Implementation(float duration) {
	DeactivateLaserTemporarily(duration);
}

void ALaser::Multicast_DeactivateLaserTemporarily_Implementation(float duration) {
	if (!HasAuthority())
		SetLaserActive(false);
}

void ALaser::ReactivateLasers() {
	if (HasAuthority()) {
		SetLaserActive(true);
		Multicast_ReactivateLaser();
	}
}

void ALaser::Multicast_ReactivateLaser_Implementation() {
	if (!HasAuthority())
		SetLaserActive(true);
}

void ALaser::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALaser, bIsLaserActive);
}

void ALaser::OnRep_IsLaserActive() {
	UpdateLaserVisuals();
}

void ALaser::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (!bIsLaserActive)
		return;

	float distance = 2500.0f;
	FVector startTrace = GetActorLocation();
	FVector endTrace = startTrace + (GetActorForwardVector() * distance);
	ECollisionChannel traceChannel = ECC_WorldStatic;

	// DrawDebugLine(GetWorld(), startTrace, endTrace, FColor::Red, false, -1.0f, 0, 1.0f);
	
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), false, this);
	RV_TraceParams.bTraceComplex = false;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.AddIgnoredActor(this);
	
	FHitResult RV_Hit;
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		RV_Hit,
		startTrace,
		endTrace,
		traceChannel,
		RV_TraceParams
	);

	FVector beamEnd = bHit ? RV_Hit.Location : endTrace; //select in blueprint
	if (niagaraLaser)
		niagaraLaser->SetVariableVec3(FName("User.BeamEnd"), beamEnd);
	
	if (bHit) {
		if (niagaraLaserImpact)
			niagaraLaserImpact->SetWorldLocation(RV_Hit.Location);
		
		if (AYellPointAndPrayCharacter* character = Cast<AYellPointAndPrayCharacter>(RV_Hit.GetActor())){
			if (HasAuthority())
				TouchingLaser(character);
		}
	}
}

