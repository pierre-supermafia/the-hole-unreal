// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "OSCManager.h"
#include "OSCServer.h"
#include "OSCClient.h"

#include "FHead.h"

#include "TheHoleOSCComponent.generated.h"

enum BodyType
{
	SKELETON,
	BLOB
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

	bool GetHeadLocation(FVector& HeadLocation);
	bool LessThanTwoBodies() const;

	UPROPERTY(EditInstanceOnly)
		FString ReceiveIPAdress;
	UPROPERTY(EditInstanceOnly)
		int32 ReceivePort;
	UPROPERTY(EditInstanceOnly)
		FString BroadcastIPAdress;
	UPROPERTY(EditInstanceOnly)
		int32 BroadcastPort;
	
	UPROPERTY(EditInstanceOnly)
		float TimeToForget;

	UPROPERTY(EditInstanceOnly)
		float MultipleBodiesWarningActivationTime;
	UPROPERTY(EditInstanceOnly)
		float MutlipleBodiesWarningDeactivationTime;

private:
	static const FOSCAddress HandshakeAddress;
	static const FOSCAddress UpdateAddress;
	static const FOSCAddress SkeletonAddress;
	static const FOSCAddress BlobAddress;
	static const FOSCAddress MultipleBodiesAlertAddress;

	UPROPERTY()
	UOSCServer* Server;
	UPROPERTY()
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
	void OnBodyReceived(const FOSCMessage& Message, BodyType Type);
	void OnMultipleBodiesDetected(const FOSCMessage& Message);

	TMap<uint8, FHead> SkeletonHeads;
	TMap<uint8, FHead> BlobHeads;

	bool GetHead(FVector& HeadLocation, BodyType Type) const;
	static const float LowerConfidenceThreshold;

	void HandleMultipleBodiesWarning(float DeltaTime);
	bool CheckMultipleBodies();
	static const float SquareDistanceThreshold;

	bool DisplayMultipleBodiesWarning = false;
	bool HasDetectedMultipleBodiesLastUpdate = false;
	float MultipleBodiesAlertLevelIncreaseSpeed;
	float MultipleBodiesAlertLevelDecreaseSpeed;
	float MultipleBodiesAlertLevel;

	float ConfidenceDecaySpeed;
	void DecayConfidences(float DeltaTime);
	void DecayConfidence(FHead& Head, float DeltaTime);
};
