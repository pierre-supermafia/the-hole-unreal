// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "TheHoleActor.generated.h"

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

public:
	USTRUCT()
		struct Head {
		GENERATED_BODY()

			FVector Position;
		float Confidence;
	};


private:
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* ScreenMesh;
	UPROPERTY(VisibleAnywhere)
		UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
		float LerpSpeed;
	UPROPERTY(VisibleAnywhere)
		float ConfidenceDecay;

	UPROPERTY(VisibleAnywhere)
		FVector2D RealScreenDimensions;

	float Scale;

	FVector Target;
};
