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

	// Referenced actors and components
	UPROPERTY(EditInstanceOnly)
	AStaticMeshActor* ScreenMesh;
	UPROPERTY(EditInstanceOnly)
	AStaticMeshActor* WarningScreenMesh;
	UPROPERTY(EditInstanceOnly)
	UTheHoleOSCComponent* OSCComponent;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;
	
	// Projection matrix modifier
	TSharedPtr<TheHoleSceneViewExtension, ESPMode::ThreadSafe> SceneViewExtensionRef;
	
	// Physical world - virtual world relationship
	UPROPERTY(EditInstanceOnly)
	FVector2D RealScreenDimensions;
	float Scale;

	// Actor movement
	FVector Target;
	bool ComputeTarget();
	UPROPERTY(EditInstanceOnly)
	float LerpSpeed;

	// Fade in and out
	void UpdateScreens();
	static const float DurationBeforeFadeOut;
	static const float FadeInStep;
	static const float FadeOutStep;
	float TimeBeforeFadeOut;
	float ScreenOpacity;
	UPROPERTY()
	UMaterialInstanceDynamic* ScreenMaterial;
	UPROPERTY()
	UMaterialInstanceDynamic* WarningScreenMaterial;
};
