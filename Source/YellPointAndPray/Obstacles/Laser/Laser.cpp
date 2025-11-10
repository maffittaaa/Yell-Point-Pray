#include "Obstacles/Laser/Laser.h"

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
		niagaraLaser->SetVariableLinearColor(FName("User.BeamColor"), FLinearColor(0.703f, 0.245f, 0.0f, 1.0f));

	if (niagaraLaserImpact)
		niagaraLaserImpact->SetVariableLinearColor(FName("User.Colour"), FLinearColor(0.703f, 0.245f, 0.0f, 1.0f));
}

void ALaser::TouchingLaser(AYellPointAndPrayCharacter* character) {
	//implementation of the alert guards
}

void ALaser::SetLaserActive(bool bActive)
{
	if (bActive && GetWorld()->GetTimerManager().IsTimerActive(reactivateTimerHandle))
		GetWorld()->GetTimerManager().ClearTimer(reactivateTimerHandle);
	
	bIsLaserActive = bActive;
    
	if (niagaraLaser) {
		niagaraLaser->ResetSystem();
		if (bActive)
			niagaraLaser->Activate();
		else {
			niagaraLaser->Deactivate();
			niagaraLaser->SetVisibility(false);
		}
	}
    
	if (niagaraLaserImpact) {
		niagaraLaserImpact->ResetSystem();
		if (bActive)
			niagaraLaserImpact->Activate();
		else {
			niagaraLaserImpact->Deactivate();
			niagaraLaserImpact->SetVisibility(false);
		}
	}
	
	if (collisionSphere)
		collisionSphere->SetCollisionEnabled(bActive ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	
	SetActorTickEnabled(bActive);
}

void ALaser::DeactivateLaserTemporarily(float duration) {
	if (!bIsLaserActive)
		return;

    UE_LOG(LogTemp, Warning, TEXT("ALaser::DeactivateTemporarily - %s for %.1f seconds"), *GetName(), duration);

	// FTimerDelegate Delegate;
	// Delegate.BindUObject(this, &ALaser::ReactivateLasers);

	SetLaserActive(false);
	SetActorTickEnabled(false);

	// GetWorld()->GetTimerManager().SetTimer(
	// 	reactivateTimerHandle,
	// 	Delegate,
	// 	duration,
	// 	false
	// );
}

void ALaser::ReactivateLasers() {
	SetLaserActive(true);
	SetActorTickEnabled(true);
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
		
		if (AYellPointAndPrayCharacter* character = Cast<AYellPointAndPrayCharacter>(RV_Hit.GetActor())) {
			if (HasAuthority())
				TouchingLaser(character);
		}
	}
}

