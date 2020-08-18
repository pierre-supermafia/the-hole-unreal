// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define TOTAL_MEASUREMENTS 3

/**
 * 
 */
class THEHOLE_API PeriodAverager
{
public:
	PeriodAverager() {}

	void AddUpdateTime(float UpdateTime);
	float AveragePeriod();

private:
	float LastUpdatesTimes[TOTAL_MEASUREMENTS];
	int MeasuredUpdateTimes = 0;
};
