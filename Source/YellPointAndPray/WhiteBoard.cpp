#include "WhiteBoard.h"
#include "YellPointAndPrayCharacter.h"
#include "Components/Button.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Net/UnrealNetwork.h"

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

	renderTarget2D = UKismetRenderingLibrary::CreateRenderTarget2D(this, width, height, RTF_RGBA16f);
	renderTarget2D->ClearColor = FColor::White;
	
	dynamicMaterialInstanceCanvas = UMaterialInstanceDynamic::Create(materialCanvas, this);
	dynamicMaterialInstanceCanvas->SetTextureParameterValue(FName("RenderTarget"), renderTarget2D);
	
	meshComp->SetMaterial(0, dynamicMaterialInstanceCanvas);
	
	dynamicMaterialInstanceBrush = UMaterialInstanceDynamic::Create(materialBrush, this);

	if (canvasTexture)
		InitializeBackground();
	
}

void AWhiteBoard::Interact_Implementation(AActor* Interactor) {
	UE_LOG(LogTemp, Warning, TEXT("Show Board"));
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

void AWhiteBoard::Draw(UTexture2D* brushTexture, float brushSize, FVector2D drawLocation, AYPPCustomPlayerState* playerState) {
	
	if (dynamicMaterialInstanceBrush) {
		UTexture2D* playerBrushTexture = brushTexture;
		if (playerState->PlayerType == EPlayerType::Blind)
			playerBrushTexture = brushTexture_P1;
		else if (playerState->PlayerType == EPlayerType::Deaf)
			playerBrushTexture = brushTexture_P2;
		else
			playerBrushTexture = brushTexture_P3;

		if (playerBrushTexture) {
			dynamicMaterialInstanceBrush->SetTextureParameterValue(FName("BrushTexture"), playerBrushTexture);
		}
	}
	
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

	FVector2D screenPosition = (size * drawLocation) - (brushSize / 2.0f); //so it gets in the center
	FVector2D screenSize = FVector2D(brushSize, brushSize);
	FVector2D coordinatePositon = FVector2D(0.0f, 0.0f);
	FVector2D coordinateSize = FVector2D(1.0f, 1.0f);
	float rotation = 0.0f;
	FVector2D pivotPoint = FVector2D(0.5f, 0.5f);
	
	if (canvas && dynamicMaterialInstanceBrush) {
		canvas->K2_DrawMaterial(
		dynamicMaterialInstanceBrush,
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

void AWhiteBoard::CloseBoard_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("Close Board"));
}

void AWhiteBoard::ClearBoard_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("Clear Board"));
}

void AWhiteBoard::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

