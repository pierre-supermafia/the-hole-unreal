// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Engine/StaticMeshActor.h"

#include "Kismet/GameplayStatics.h"

#include "TheHoleActor.generated.h"

USTRUCT()
struct FHead {
	GENERATED_BODY()

		FVector Position;
	float Confidence;
};

UCLASS()
class THEHOLE_API ATheHoleActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATheHoleActor();

	void SetSkeletonData(int index, FVector Position, float confidence);
	void SetBlobData(FVector Position);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditInstanceOnly)
		AStaticMeshActor* ScreenMesh;
	UPROPERTY(VisibleAnywhere)
		UCameraComponent* Camera;

	UPROPERTY(EditInstanceOnly)
		float LerpSpeed;
	UPROPERTY(EditInstanceOnly)
		float ConfidenceDecay;

	UPROPERTY(EditInstanceOnly)
		FVector2D RealScreenDimensions;

	float Scale;

	TMap<uint8, FHead> Heads;
	TMap<uint8, FVector> Blobs;

	FVector ComputeTarget() const;
};
