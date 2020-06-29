// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SceneViewExtension.h"

class ATheHoleActor;

/**
 * 
 */
class TheHoleSceneViewExtension : public FSceneViewExtensionBase
{
public:
	TheHoleSceneViewExtension(const FAutoRegister& AutoRegister, ATheHoleActor* TheHoleActor);
	~TheHoleSceneViewExtension();

	// Implemented interface methods
	virtual void SetupViewProjectionMatrix(FSceneViewProjectionData& InOutProjectionData) override;
	virtual int32 GetPriority() const override { return 0; }

	// Unimplemented interface methods
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override {}
	virtual void PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override {};
	virtual void PostRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override {};

private:
	ATheHoleActor* TheHoleActor;

	void ComputeMatrices(
		FMatrix& ProjectionMatrix,
		FMatrix& RotationMatrix,
		FVector& TranslationComponent) const;

};
