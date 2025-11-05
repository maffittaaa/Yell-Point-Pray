#include "WhiteBoard.h"
#include "YellPointAndPrayCharacter.h"
#include "Engine/Canvas.h"
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


void AWhiteBoard::ServerSetMaterial()
{
	meshComp->SetMaterial(0, dynamicMaterialInstanceCanvas);
}

void AWhiteBoard::BeginPlay() {
	Super::BeginPlay();
	

	int width = 1024;
	int height = 1024;

	renderTarget2D = UKismetRenderingLibrary::CreateRenderTarget2D(this, width, height, RTF_RGBA16f);
	renderTarget2D->ClearColor = FColor::White;
	
	dynamicMaterialInstanceCanvas = UMaterialInstanceDynamic::Create(materialCanvas, this);
	dynamicMaterialInstanceCanvas->SetTextureParameterValue(FName("RenderTarget"), renderTarget2D);
	
	ServerSetMaterial();
	
	dynamicMaterialInstanceBrush = UMaterialInstanceDynamic::Create(materialBrush, this);

	if (canvasTexture)
		InitializeBackground();
}



void AWhiteBoard::Interact_Implementation(AActor* Interactor) {
	UE_LOG(LogTemp, Warning, TEXT("Show Board"));

	AYellPointAndPrayCharacter* playerCharacter = Cast<AYellPointAndPrayCharacter>(Interactor);
	if (!playerCharacter) return;

	APlayerController* playerController = Cast<APlayerController>(playerCharacter->GetController());
	if (!playerController) return;
}

void AWhiteBoard::InitializeBackground() {
	UCanvas* canvas = nullptr;
	FVector2D size;
	FDrawToRenderTargetContext context;
    
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(
		this,
		renderTarget2D,
		canvas,
		size,
		context
	);

	if (canvas) {
		canvas->K2_DrawTexture(
			canvasTexture,
			FVector2D(0.0f, 0.0f),
			size,
			FVector2D(0.0f, 0.0f),
			FVector2D(1.0f, 1.0f),
			FLinearColor::White,
			BLEND_Translucent,
			0.0f,
			FVector2D(0.0f, 0.0f)
		);
	}
    
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, context);
}

void AWhiteBoard::Draw(UTexture2D* brushTexture, float brushSize, FVector2D drawLocation) {
	if (dynamicMaterialInstanceBrush)
		dynamicMaterialInstanceBrush->SetTextureParameterValue(FName("BrushTexture"), brushTexture);
	
	UCanvas* canvas = nullptr;
	FVector2D size;
	FDrawToRenderTargetContext context;
	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(
		this,
		renderTarget2D,
		canvas,
		size,
		context
		);

	UE_LOG(LogTemp, Warning, TEXT("Canvas: %p"), canvas);

	FVector2D screenPosition = (size * drawLocation) - (brushSize / 2.0f); //so it gets in the center
	FVector2D screenSize = FVector2D(brushSize, brushSize);
	FVector2D coordinatePositon = FVector2D(0.0f, 0.0f);
	FVector2D coordinateSize = FVector2D(1.0f, 1.0f);
	float rotation = 0.0f;
	FVector2D pivotPoint = FVector2D(0.5f, 0.5f);
	
	if (canvas && dynamicMaterialInstanceBrush) {
		canvas->K2_DrawMaterial(
		materialBrush,
		screenPosition,
		screenSize,
		coordinatePositon,
		coordinateSize,
		rotation,
		pivotPoint
		);
	}
	
	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, context);
}


void AWhiteBoard::CloseBoard() {
	UE_LOG(LogTemp, Warning, TEXT("Close Board"));
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	if (!playerController) return;
}

void AWhiteBoard::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

