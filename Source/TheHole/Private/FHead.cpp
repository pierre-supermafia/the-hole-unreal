// Fill out your copyright notice in the Description page of Project Settings.


#include "FHead.h"

FHead::FHead() :
	Position(FVector()),
	Confidence(0)
{}

FHead::FHead(FVector Position, float Confidence) :
	Position(Position),
	Confidence(Confidence)
{}

