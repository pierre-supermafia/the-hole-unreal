// Fill out your copyright notice in the Description page of Project Settings.


#include "HolePlayer.h"

FSceneView* UHolePlayer::CalcSCeneView(class FSceneViewFamily* ViewFamily,
	FVector& OutViewLocation,
	FRotator& OutViewRotation,
	FViewport* Viewport,
	class FViewElementDrawer* ViewDrawer,
	EStereoscopicPass StereoPass)
{
	FSceneView* View = ULocalPlayer::CalcSCeneView(ViewFamily, OutViewLocation, OutViewRotation, Viewport, ViewDrawer, StereoPass);
	
	if (View)
	{
		FMatrix ProjectionMatrix = ComputeMatrix();

		View->UpdateProjectionMatrix(ProjectionMatrix);
	}

	return View;
}

FMatrix UHolePlayer::ComputeMatrix()
{
	// TODO
	return FMatrix();
}