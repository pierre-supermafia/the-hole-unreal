// Fill out your copyright notice in the Description page of Project Settings.


#include "TheHoleActor.h"

// Sets default values
ATheHoleActor::ATheHoleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
}

/**
*/
void ATheHoleActor::SetSkeletonData(int index, FVector Position, float confidence)
{
}

/**
*/
void ATheHoleActor::SetBlobData(FVector Position)
{
}

// Called when the game starts or when spawned
void ATheHoleActor::BeginPlay()
{
	Super::BeginPlay();

	FVector ScreenDimensions = ScreenMesh->GetActorScale();
	Scale = 0.5f * ScreenDimensions.X / RealScreenDimensions.X
		+ 0.5f * ScreenDimensions.Y / RealScreenDimensions.Y;

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

	FVector Target = ScreenMesh->GetActorLocation() + ComputeTarget();

	SetActorLocation(Target);
}

/**
*/
FVector ATheHoleActor::ComputeTarget() const
{
	// DEBUG : until OSC is implemented, the camera just rotates around the screen
	const float speed = 0.5f;
	float t = speed * GetGameTimeSinceCreation();
	const float radius = 5;
	return FVector(
		radius * cosf(t),
		radius * sinf(t),
		radius
	);
}

