// Fill out your copyright notice in the Description page of Project Settings.

#include "TheHoleActor.h"

const float ATheHoleActor::FadeInStep = 0.02;
const float ATheHoleActor::FadeOutStep = 0.1;
const float ATheHoleActor::DurationBeforeFadeOut = 0.5f;

// Sets default values
ATheHoleActor::ATheHoleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	OSCComponent = CreateDefaultSubobject<UTheHoleOSCComponent>(TEXT("OSC"));
	
	Target = FVector(0.0f, 0.0f, 1.8f);
	ScreenOpacity = 0.0f;
	TimeBeforeFadeOut = DurationBeforeFadeOut;
}

/**
 * Gives the three corners of the screen necessary to the projection calculations
 * 
 * @param pa Vector in which the lower left point will be stored
 * @param pa Vector in which the lower right point will be stored
 * @param pa Vector in which the upper left point will be stored
 */
void ATheHoleActor::GetScreenCorners(FVector& pa, FVector& pb, Vector& pc) const
{
	FTransform Transform = ScreenMesh->GetTransform();

	pa = Transform.TransformPosition(FVector(-50.0f, 50.0f, 0.0f));
	pb = Transform.TransformPosition(FVector(50.0f, 50.0f, 0.0f));
	pc = Transform.TransformPosition(FVector(-50.0f, -50.0f, 0.0f));
}

// Called when the game starts or when spawned
void ATheHoleActor::BeginPlay()
{
	Super::BeginPlay();

	// Prepare the screen to have its opacity changed
	auto Mesh = ScreenMesh->GetStaticMeshComponent();
	auto Material = Mesh->GetMaterial(0);
	ScreenMaterial = UMaterialInstanceDynamic::Create(Material, NULL);
	Mesh->SetMaterial(0, ScreenMaterial);

	// Prepare the warning mesh to have its opacity changed
	Mesh = WarningScreenMesh->GetStaticMeshComponent();
	Material = Mesh->GetMaterial(0);
	WarningScreenMaterial = UMaterialInstanceDynamic::Create(Material, NULL);
	Mesh->SetMaterial(0, WarningScreenMaterial);

	// Compute the real world to virtual world scale
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
		TimeBeforeFadeOut = DurationBeforeFadeOut;
	}
	else
	{
		TimeBeforeFadeOut -= DeltaTime;
	}

	SetActorLocation(FMath::Lerp(
		GetActorLocation(),
		Scale * Target + ScreenMesh->GetActorLocation(),
		LerpSpeed
	));

	UpdateScreens();

	// Camera perspective
	if (!SceneViewExtensionRef.IsValid())
	{
		SceneViewExtensionRef = FSceneViewExtensions::NewExtension<TheHoleSceneViewExtension, ATheHoleActor*>(this);
	}
}

/**
 * Consults the UTheHoleOSCComponent to have the head's position. Returns true if a head was given.
 */
bool ATheHoleActor::ComputeTarget()
{
	if (IsValid(OSCComponent) && !OSCComponent->GetHeadLocation(Target))
	{
		return false;
	}
	return true;
}

/**
 * Update the two screens' opacity
 * For fading in and out, and for the warning display
 */
void ATheHoleActor::UpdateScreens()
{
	// Multiple bodies warning
	if (OSCComponent->LessThanTwoBodies())
	{
		WarningScreenMaterial->SetScalarParameterValue(TEXT("Opacity"), 0.0f);
	}
	else
	{
		WarningScreenMaterial->SetScalarParameterValue(TEXT("Opacity"), 1.0f);
		// Instantly trigger fade out
		TimeBeforeFadeOut = 0.0f;
	}

	if (TimeBeforeFadeOut <= 0)
	{
		ScreenOpacity += FadeOutStep;
	}
	else
	{
		ScreenOpacity -= FadeInStep;
	}

	ScreenOpacity = FMath::Clamp(ScreenOpacity, 0.0f, 1.0f);
	ScreenMaterial->SetScalarParameterValue(TEXT("Opacity"), ScreenOpacity);
}

