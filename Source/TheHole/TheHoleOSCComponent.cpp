// Fill out your copyright notice in the Description page of Project Settings.


#include "TheHoleOSCComponent.h"

const FOSCAddress UTheHoleOSCComponent::HandshakeAddress = FOSCAddress("/ks/request/handshake");
const FOSCAddress UTheHoleOSCComponent::UpdateAddress = FOSCAddress("/ks/request/update");
const FOSCAddress UTheHoleOSCComponent::SkeletonAddress= FOSCAddress("/ks/server/track/skeleton/head");
const FOSCAddress UTheHoleOSCComponent::BlobAddress = FOSCAddress("/ks/server/track/headblob");
const FOSCAddress UTheHoleOSCComponent::MultipleBodiesAlertAddress = FOSCAddress("/ks/server/track/multiple-bodies");

const float UTheHoleOSCComponent::UpdatePeriod = 9.0f;
const float UTheHoleOSCComponent::SquareDistanceThreshold = 1.5f * 1.5f;
const float UTheHoleOSCComponent::LowerConfidenceThreshold = 0.5f;

// Sets default values for this component's properties
UTheHoleOSCComponent::UTheHoleOSCComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MultipleBodiesAlertLevel = 0;

	// Decrease is always applied, so Increase speed must compensate
	MultipleBodiesAlertLevelDecreaseSpeed = 1.0f / MutlipleBodiesWarningDeactivationTime;
	MultipleBodiesAlertLevelIncreaseSpeed = 1.0f / MultipleBodiesWarningActivationTime;
}


// Called when the game starts
void UTheHoleOSCComponent::BeginPlay()
{
	Super::BeginPlay();

	CreateMessages();
	InitOSC();
	SendHandshake();	
}

void UTheHoleOSCComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);

	Server->Stop();
}


// Called every frame
void UTheHoleOSCComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DecayConfidences();

	HandleMultipleBodiesWarning(DeltaTime);
}

void UTheHoleOSCComponent::HandleMultipleBodiesWarning(float DeltaTime)
{
	if (MultipleBodiesAlertLevel > 1.0f)
	{
		DisplayMultipleBodiesWarning = true;
		MultipleBodiesAlertLevel = 1.0f;
	}
	else if (MultipleBodiesAlertLevel < 0.0f)
	{
		DisplayMultipleBodiesWarning = false;
		MultipleBodiesAlertLevel = 0.0f;
	}

	// Bypass costly check if the sensors already report on multiple bodies
	HasDetectedMultipleBodiesLastUpdate = HasDetectedMultipleBodiesLastUpdate || CheckMultipleBodies();
	
	if (HasDetectedMultipleBodiesLastUpdate)
	{
		MultipleBodiesAlertLevel += MultipleBodiesAlertLevelIncreaseSpeed * DeltaTime;
		HasDetectedMultipleBodiesLastUpdate = false;
	}
	else
	{
		MultipleBodiesAlertLevel -= MultipleBodiesAlertLevelDecreaseSpeed * DeltaTime;
	}
}

bool UTheHoleOSCComponent::GetHeadLocation(FVector& HeadLocation)
{
	if (LessThanTwoBodies())
	{
		CheckMultipleBodies();
	}
	if (! LessThanTwoBodies())
	{
		return false;
	}
	return GetHead(HeadLocation, SKELETON) || GetHead(HeadLocation, BLOB);
}

bool UTheHoleOSCComponent::LessThanTwoBodies() const
{
	return MultipleBodiesAlertLevel <= 0.0f;
}

bool UTheHoleOSCComponent::GetHead(FVector& HeadLocation, BodyType Type) const
{
	float TotalConfidence = 0;
	FVector TotalPosition = FVector(0, 0, 0);
	auto Collection = (Type == SKELETON) ? SkeletonHeads : BlobHeads;

	for (auto it = Collection.CreateConstIterator(); it; ++it)
	{
		TotalConfidence += it->Value.Confidence;
		TotalPosition += it->Value.Position * it->Value.Confidence;
	}

	if (TotalConfidence > LowerConfidenceThreshold)
	{
		if (Type == BLOB)
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, TEXT("USING BLOBS"));
		}
		HeadLocation = TotalPosition / TotalConfidence;
		return true;
	}
	return false;
}


bool UTheHoleOSCComponent::CheckMultipleBodies()
{
	for (auto it1 = SkeletonHeads.CreateConstIterator(); it1; ++it1)
	{
		if (it1->Value.Confidence < LowerConfidenceThreshold)
		{
			continue;
		}
		// Check among skeletons
		for (auto it2 = it1; it2; ++it2)
		{
			if (it1 == it2 || it2->Value.Confidence < LowerConfidenceThreshold)
			{
				continue;
			}
			float SquareDistance = FVector::DistSquared(
				it1->Value.Position,
				it2->Value.Position
			);
			if (SquareDistance < SquareDistanceThreshold) {
				return true;
			}
		}

		// Check between skeletons and blobs
		for (auto it2 = BlobHeads.CreateConstIterator(); it2; ++it2)
		{
			if (it2->Value.Confidence < LowerConfidenceThreshold)
			{
				continue;
			}
			float SquareDistance = FVector::DistSquared(
				it1->Value.Position,
				it2->Value.Position
			);
			if (SquareDistance < SquareDistanceThreshold) {
				return true;
			}
		}
	}

	// Check among blobs
	for (auto it1 = BlobHeads.CreateConstIterator(); it1; ++it1)
	{
		if (it1->Value.Confidence < LowerConfidenceThreshold)
		{
			continue;
		}
		for (auto it2 = it1; it2; ++it2)
		{
			if (it1 == it2 || it2->Value.Confidence < LowerConfidenceThreshold)
			{
				continue;
			}
			float SquareDistance = FVector::DistSquared(
				it1->Value.Position,
				it2->Value.Position
			);
			if (SquareDistance < SquareDistanceThreshold) {
				return true;
			}
		}
	}

	return false;
}

void UTheHoleOSCComponent::DecayConfidences()
{
	for (auto it = SkeletonHeads.CreateIterator(); it; ++it)
	{
		it.Value().Confidence *= ConfidenceDecay;
	}

	for (auto it = BlobHeads.CreateIterator(); it; ++it)
	{
		it.Value().Confidence *= ConfidenceDecay;
	}
}


void UTheHoleOSCComponent::InitOSC()
{
	Client = UOSCManager::CreateOSCClient(BroadcastIPAdress, BroadcastPort, "TheHoleClient");

	Server = UOSCManager::CreateOSCServer(ReceiveIPAdress, ReceivePort, false, true, "TheHoleServer");
	Server->OnOscMessageReceived.AddDynamic(this, &UTheHoleOSCComponent::OnMessageReceived);
}

void UTheHoleOSCComponent::CreateMessages()
{
	HandshakeMessage.SetAddress(HandshakeAddress);
	HandshakeMessage = UOSCManager::AddInt32(HandshakeMessage, (int32)ReceivePort);

	UpdateMessage.SetAddress(UpdateAddress);
	UpdateMessage = UOSCManager::AddInt32(UpdateMessage, (int32)ReceivePort);
}

void UTheHoleOSCComponent::SendHandshake()
{
	Client->SendOSCMessage(HandshakeMessage);

	// After the handshake, send regular update requests
	// so that the trackers don't forget about us
	GetWorld()->GetTimerManager().SetTimer(
		UpdateTimerHandle,
		this,
		&UTheHoleOSCComponent::SendUpdate,
		UpdatePeriod,
		true
	);
}

void UTheHoleOSCComponent::SendUpdate()
{
	Client->SendOSCMessage(UpdateMessage);
}

void UTheHoleOSCComponent::OnMessageReceived(const FOSCMessage& Message, const FString& IPAddress, int32 Port)
{
	const FOSCAddress& Address = Message.GetAddress();
	if (Address.Matches(SkeletonAddress))
	{
		OnBodyReceived(Message, SKELETON);
	}
	else if (Address.Matches(BlobAddress))
	{
		OnBodyReceived(Message, BLOB);
	}
	else if (Address.Matches(MultipleBodiesAlertAddress))
	{
		OnMultipleBodiesDetected(Message);
	}
}

void UTheHoleOSCComponent::OnBodyReceived(const FOSCMessage& Message, BodyType Type)
{
	int32 id;
	float x, y, z;
	UOSCManager::GetInt32(Message, 0, id);

	UOSCManager::GetFloat(Message, 1, x);
	UOSCManager::GetFloat(Message, 2, y);
	UOSCManager::GetFloat(Message, 3, z);

	if (Type == SKELETON)
	{
		float conf;
		UOSCManager::GetFloat(Message, 4, conf);

		SkeletonHeads.Add(id, FHead(FVector(x, -y, z), conf));
	}
	else
	{
		BlobHeads.Add(id, FHead(FVector(x, -y, z), 1.0f));
	}

}

void UTheHoleOSCComponent::OnMultipleBodiesDetected(const FOSCMessage& Message)
{
	HasDetectedMultipleBodiesLastUpdate = true;
}

