#include "WhiteBoard.h"
#include "YellPointAndPrayCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetRenderingLibrary.h"

AWhiteBoard::AWhiteBoard() {
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	meshComp->SetupAttachment(RootComponent);

	dynamicMaterialInstanceCanvas = nullptr;
	dynamicMaterialInstanceBrush = nullptr;
}

void AWhiteBoard::BeginPlay() {
	Super::BeginPlay();

	int width = 1024;
	int height = 1024;

	renderTarget2D = UKismetRenderingLibrary::CreateRenderTarget2D(this, 1024, 1024, RTF_RGBA8);
	renderTarget2D->ClearColor = FColor::White;

	dynamicMaterialInstanceCanvas = UMaterialInstanceDynamic::Create(materialCanvas, this);
	dynamicMaterialInstanceCanvas->SetTextureParameterValue(FName("RenderTarget"), renderTarget2D);
	
	meshComp->SetMaterial(0, dynamicMaterialInstanceCanvas);
	
	dynamicMaterialInstanceBrush = UMaterialInstanceDynamic::Create(materialBrush, this);
}

void AWhiteBoard::Interact_Implementation(AActor* Interactor) {
	UE_LOG(LogTemp, Warning, TEXT("Show Board"));

	AYellPointAndPrayCharacter* playerCharacter = Cast<AYellPointAndPrayCharacter>(Interactor);
	if (!playerCharacter) return;

	APlayerController* playerController = Cast<APlayerController>(playerCharacter->GetController());
	if (!playerController) return;
}

void AWhiteBoard::Draw(UTexture2D* brushTexture, float brushSize, FVector2D* drawLocation) {
	if (dynamicMaterialInstanceBrush)
		dynamicMaterialInstanceBrush->SetTextureParameterValue(FName("BrushTexture"), brushTexture);

	// renderTarget2D->BeginDrawCanvasToRenderTarget();
}


void AWhiteBoard::CloseBoard() {
	UE_LOG(LogTemp, Warning, TEXT("Close Board"));
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	if (!playerController) return;
}

void AWhiteBoard::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

