// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PeriodAverager.h"

#include "FHead.generated.h"

/**
 * 
 */
USTRUCT()
struct FHead {
	GENERATED_BODY()

	FHead();
	FHead(FVector Position, float Confidence);

	FVector Position;
	float Confidence;
	PeriodAverager PeriodAverager;
};
