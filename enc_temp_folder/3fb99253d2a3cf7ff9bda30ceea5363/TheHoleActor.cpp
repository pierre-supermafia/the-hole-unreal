// Fill out your copyright notice in the Description page of Project Settings.


#include "TheHoleActor.h"

// Sets default values
ATheHoleActor::ATheHoleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	OSCComponent = CreateDefaultSubobject<UTheHoleOSCComponent>(TEXT("OSC"));
	
	Target = FVector(0.0f, 0.0f, 1.8f);
}

void ATheHoleActor::GetScreenCorners(FVector& pa, FVector& pb, FVector& pc) const
{
	FTransform Transform = ScreenMesh->GetTransform();
	// Bottom-left corner
	pa = Transform.TransformPosition(FVector(-50.0f, 50.0f, 0.0f));
	// Bottom-right corner
	pb = Transform.TransformPosition(FVector(50.0f, 50.0f, 0.0f));
	// Top-left corner
	pc = Transform.TransformPosition(FVector(-50.0f, -50.0f, 0.0f));
}

// Called when the game starts or when spawned
void ATheHoleActor::BeginPlay()
{
	Super::BeginPlay();

	auto Mesh = ScreenMesh->GetStaticMeshComponent();
	auto Material = Mesh->GetMaterial(0);
	ScreenMaterial = UMaterialInstanceDynamic::Create(Material, NULL);
	Mesh->SetMaterial(0, ScreenMaterial);

	FVector ScreenDimensions = ScreenMesh->GetActorScale();
	Scale = 0.5f * ScreenDimensions.X / RealScreenDimensions.X
		+ 0.5f * ScreenDimensions.Y / RealScreenDimensions.Y;
	Scale *= 100; // PlaneMeshes have sides 100 units long 

	// Set this camera as the view target
	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (OurPlayerController)
	{
		OurPlayerController->SetViewTarget(this);
	}
}

// Called every frame
void ATheHoleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Camera location
	if (ComputeTarget())
	{
		// Fade in
		ScreenMaterial->SetScalarParameterValue(TEXT("Opacity"), 0.0f);
		UE_LOG(LogTemp, Log, TEXT("FADE IN"));
	}
	else
	{
		// No body detected : fade out
		ScreenMaterial->SetScalarParameterValue(TEXT("Opacity"), 1.0f);
		UE_LOG(LogTemp, Log, TEXT("FADE OUT"));
	}

	SetActorLocation(FMath::Lerp(
		GetActorLocation(),
		Scale * Target + ScreenMesh->GetActorLocation(),
		LerpSpeed
	));

	// Camera perspective
	if (!SceneViewExtensionRef.IsValid())
	{
		SceneViewExtensionRef = FSceneViewExtensions::NewExtension<TheHoleSceneViewExtension, ATheHoleActor*>(this);
	}
}

/**
*/
bool ATheHoleActor::ComputeTarget()
{
	if (IsValid(OSCComponent) && !OSCComponent->GetHeadLocation(Target))
	{
		return false;
	}
	return true;
}

