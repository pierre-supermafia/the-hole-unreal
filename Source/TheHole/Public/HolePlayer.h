// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "HolePlayer.generated.h"

/**
 * 
 */
UCLASS()
class THEHOLE_API UHolePlayer : public ULocalPlayer
{
	GENERATED_BODY()

		FSceneView* CalcSceneView(class FSceneViewFamily* ViewFamily,
			FVector& OutViewLocation,
			FRotator& OutViewRotation,
			FViewport* Viewport,
			class FViewElementDrawer* ViewDrawer = NULL,
			EStereoscopicPass StereoPass = eSSP_FULL) override;
		
		FMatrix ComputeMatrix();
};
