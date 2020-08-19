// Fill out your copyright notice in the Description page of Project Settings.


#include "PeriodAverager.h"

void PeriodAverager::AddUpdateTime(float UpdateTime)
{
	for (int i = MeasuredUpdateTimes; i > 0; i--)
	{
		LastUpdatesTimes[i] = LastUpdatesTimes[i - 1];
	}
	LastUpdatesTimes[0] = UpdateTime;

	if (MeasuredUpdateTimes < TOTAL_MEASUREMENTS)
	{
		MeasuredUpdateTimes++;
	}
}

float PeriodAverager::AveragePeriod()
{
	if (MeasuredUpdateTimes > 1)
	{
		float TotalDifference = LastUpdatesTimes[0] - LastUpdatesTimes[MeasuredUpdateTimes - 1];
		return TotalDifference / (MeasuredUpdateTimes - 1);
	}

	// Not enough measurements
	return -1.0f;
}
