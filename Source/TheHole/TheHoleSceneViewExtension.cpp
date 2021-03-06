// Fill out your copyright notice in the Description page of Project Settings.


#include "TheHoleSceneViewExtension.h"
#include "TheHoleActor.h"

/**
 * CTOR
 */
TheHoleSceneViewExtension::TheHoleSceneViewExtension(const FAutoRegister& AutoRegister, ATheHoleActor* TheHoleActor)
	: FSceneViewExtensionBase(AutoRegister)
	, TheHoleActor(TheHoleActor)
{
	// Initialize the constant "z-fixer" matrix
	ZAxisBoundTransform.SetIdentity();
	ZAxisBoundTransform.M[2][2] = 0.5f;
	ZAxisBoundTransform.M[3][2] = 0.5f;
}

/**
 * DTOR
 */
TheHoleSceneViewExtension::~TheHoleSceneViewExtension() {}

/**
 * Overriden method
 */
void TheHoleSceneViewExtension::SetupViewProjectionMatrix(FSceneViewProjectionData& InOutProjectionData)
{
	ComputeMatrices(
		InOutProjectionData.ProjectionMatrix,
		InOutProjectionData.ViewRotationMatrix,
		InOutProjectionData.ViewOrigin
		);
}

/**
 * Uses the ATheHoleActor's state to compute the required projection data
 */
void TheHoleSceneViewExtension::ComputeMatrices(
	FMatrix& ProjectionMatrix,
	FMatrix& RotationMatrix,
	FVector& TranslationComponent
) const 
{
	// The code is based on Robert Kooima's publication  
	// "Generalized Perspective Projection," 2009, 
	// http://csc.lsu.edu/~kooima/pdfs/gen-perspective.pdf 

	// Points of interest
	FVector pe = TheHoleActor->GetActorLocation();
	FVector pa, pb, pc;
	TheHoleActor->GetScreenCorners(pa, pb, pc);

	// Camera to screen corners vectors
	FVector va, vb, vc;
	va = pa - pe;
	vb = pb - pe;
	vc = pc - pe;

	// Orthonormal basis for the screen
	FVector vr, vu, vn; // right, up, normal
	vr = pb - pa;
	vr.Normalize();
	vu = pc - pa;
	vu.Normalize();
	vn = -FVector::CrossProduct(vr, vu); // Unreal's negative orientation

	// Rotation matrix (points away from the screen)
	RotationMatrix.SetIdentity();
	RotationMatrix.M[0][0] = -vr.X; RotationMatrix.M[0][1] = -vr.Y; RotationMatrix.M[0][2] = -vr.Z;
	RotationMatrix.M[1][0] = -vu.X; RotationMatrix.M[1][1] = -vu.Y; RotationMatrix.M[1][2] = -vu.Z;
	RotationMatrix.M[2][0] = -vn.X; RotationMatrix.M[2][1] = -vn.Y; RotationMatrix.M[2][2] = -vn.Z;

	// Clip plane distances
	float n = GNearClippingPlane;
	float f = 5000.0f;
	float nd = n / -FVector::DotProduct(va, vn);

	float l = FVector::DotProduct(vr, va) * nd;
	float r = FVector::DotProduct(vr, vb) * nd;
	float b = FVector::DotProduct(vu, va) * nd;
	float t = FVector::DotProduct(vu, vc) * nd;

	// Note : The w-component of a transformed world vector is z instead of -z
	// Hence M[2][3] = 1, and all the negative signs before everything else
	ProjectionMatrix.SetIdentity();
	ProjectionMatrix.M[0][0] = -2.0f * n / (r - l);
	ProjectionMatrix.M[1][1] = -2.0f * n / (t - b);
	ProjectionMatrix.M[2][0] = -(r + l) / (r - l);
	ProjectionMatrix.M[2][1] = -(t + b) / (t - b);
	ProjectionMatrix.M[2][2] = n / (f - n);
	ProjectionMatrix.M[2][3] = 1.0f;
	ProjectionMatrix.M[3][2] = (f * n) / (f - n);
	ProjectionMatrix.M[3][3] = 0.0f;

	// Converts the standard [-1, 1] camera space into
	// UE's [0, 1] camera space
	ProjectionMatrix = ProjectionMatrix * ZAxisBoundTransform;

	TranslationComponent = pe;
}