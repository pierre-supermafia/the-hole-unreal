// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Engine/StaticMeshActor.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TheHoleSceneViewExtension.h"

#include "TheHoleOSCComponent.h"

#include "TheHoleActor.generated.h"

UCLASS()
class THEHOLE_API ATheHoleActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATheHoleActor();

	void GetScreenCorners(FVector& pa, FVector& pb, FVector& pc) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditInstanceOnly)
		AStaticMeshActor* ScreenMesh;
	UPROPERTY(EditInstanceOnly)
		UTheHoleOSCComponent* OSCComponent;
	UPROPERTY(VisibleAnywhere)
		UCameraComponent* Camera;
	
	TSharedPtr<TheHoleSceneViewExtension, ESPMode::ThreadSafe> SceneViewExtensionRef;

	UPROPERTY(EditInstanceOnly)
		float LerpSpeed;
	

	UPROPERTY(EditInstanceOnly)
		FVector2D RealScreenDimensions;

	float Scale;

	FVector Target;
	bool ComputeTarget();

	UMaterialInstanceDynamic* ScreenMaterial;
};
