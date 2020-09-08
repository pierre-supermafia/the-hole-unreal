// Fill out your copyright notice in the Description page of Project Settings.


#include "FHead.h"

FHead::FHead() :
	FHead(FVector(), -1.0f)
{}

FHead::FHead(FVector Position, float Confidence) :
	Position(Position),
	Confidence(Confidence),
	PeriodAverager()
{}

