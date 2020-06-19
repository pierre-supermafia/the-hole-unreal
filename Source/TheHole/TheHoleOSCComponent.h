// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "OSCManager.h"
#include "OSCServer.h"
#include "OSCClient.h"

#include "TheHoleOSCComponent.generated.h"

USTRUCT()
struct FHead {
	GENERATED_BODY()

	FHead() :
		Position(FVector()),
		Confidence(0) {}

	FHead(FVector Position, float Confidence) :
		Position(Position),
		Confidence(Confidence) {}

	FVector Position;
	float Confidence;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class THEHOLE_API UTheHoleOSCComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTheHoleOSCComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	bool GetHeadLocation(FVector& HeadLocation) const;
	void DecayConfidences();

	UPROPERTY(EditInstanceOnly)
		FString ReceiveIPAdress;
	UPROPERTY(EditInstanceOnly)
		int32 ReceivePort;
	UPROPERTY(EditInstanceOnly)
		FString BroadcastIPAdress;
	UPROPERTY(EditInstanceOnly)
		int32 BroadcastPort;
	UPROPERTY(EditInstanceOnly)
		float ConfidenceDecay;

private:
	static const FOSCAddress HandshakeAddress;
	static const FOSCAddress UpdateAddress;
	static const FOSCAddress SkeletonAddress;
	static const FOSCAddress BlobAddress;

	UOSCServer* Server;
	UOSCClient* Client;

	FOSCMessage HandshakeMessage;
	FOSCMessage UpdateMessage;

	void InitOSC();
	void CreateMessages();

	void SendHandshake();
	UFUNCTION()
	void SendUpdate();
	FTimerHandle UpdateTimerHandle;
	static const float UpdatePeriod;

	FOSCDispatchMessageEvent OnSkeletonReceivedDelegate;
	FOSCDispatchMessageEvent OnBlobReceivedDelegate;

	UFUNCTION()
	void OnMessageReceived(const FOSCMessage& Message, const FString& IPAddress, int32 Port);
	void OnSkeletonReceived(const FOSCMessage& Message);
	void OnBlobReceived(const FOSCMessage& Message);

	TMap<uint8, FHead> Heads;
	TMap<uint8, FVector> Blobs;

	static const float LowerConfidenceThreshold;
};
