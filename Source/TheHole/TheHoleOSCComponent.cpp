// Fill out your copyright notice in the Description page of Project Settings.


#include "TheHoleOSCComponent.h"

const FOSCAddress UTheHoleOSCComponent::HandshakeAddress = FOSCAddress("/ks/request/handshake");
const FOSCAddress UTheHoleOSCComponent::UpdateAddress = FOSCAddress("/ks/request/update");
const FOSCAddress UTheHoleOSCComponent::SkeletonAddress= FOSCAddress("/ks/server/track/skeleton/head");
const FOSCAddress UTheHoleOSCComponent::BlobAddress = FOSCAddress("/ks/server/track/headblob");
const FOSCAddress UTheHoleOSCComponent::MultipleBodiesAlertAddress = FOSCAddress("/ks/server/track/multiple-bodies");

const float UTheHoleOSCComponent::UpdatePeriod = 9.0f;
const float UTheHoleOSCComponent::SquareDistanceThreshold = 1.5f * 1.5f;
const float UTheHoleOSCComponent::MultipleBodiesWarningDuration = 1.5f;
const float UTheHoleOSCComponent::LowerConfidenceThreshold = 0.5f;

// Sets default values for this component's properties
UTheHoleOSCComponent::UTheHoleOSCComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MultipleBodiesWarningTimer = 0;
}


// Called when the game starts
void UTheHoleOSCComponent::BeginPlay()
{
	Super::BeginPlay();

	CreateMessages();
	InitOSC();
	SendHandshake();	
}

// Called when the game ends
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

	if (MultipleBodiesWarningTimer > 0)
	{
		MultipleBodiesWarningTimer -= DeltaTime;
	}
}

/**
 * Computes the user's head location, and sets it in the given reference
 * if the result is convincing enough. Returns true if it is the case.
 * Return false if either no head or more than one was found
 * 
 * @param HeadLocation Vector in which to store the location if any is found
 */
bool UTheHoleOSCComponent::GetHeadLocation(FVector& HeadLocation)
{
	if (LessThanTwoBodies())
	{
		CheckMultipleBodies();
	}
	if (!LessThanTwoBodies())
	{
		return false;
	}
	return GetHead(HeadLocation, SKELETON) || GetHead(HeadLocation, BLOB);
}

/**
 * Returns true if the warning timer for multiple bodies is not ellapsed
 */
bool UTheHoleOSCComponent::LessThanTwoBodies() const
{
	return MultipleBodiesWarningTimer <= 0;
}

/**
 * Computes the user's head location using only skeletons or blobs and
 * return true if the result is convincing. Return false otherwise.
 * 
 * @param HeadLocation Vector in which to store the location if any is found
 * @param Type Either skeletons of blobs
 */
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
		HeadLocation = TotalPosition / TotalConfidence;
		return true;
	}
	return false;
}

/**
 * Checks if mutliple bodies are being detected by distinct cameras
 */
void UTheHoleOSCComponent::CheckMultipleBodies()
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
			if (SquareDistance > SquareDistanceThreshold) {
				MultipleBodiesWarningTimer = MultipleBodiesWarningDuration;
				return;
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
			if (SquareDistance > SquareDistanceThreshold) {
				MultipleBodiesWarningTimer = MultipleBodiesWarningDuration;
				return;
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
			if (SquareDistance > SquareDistanceThreshold) {
				MultipleBodiesWarningTimer = MultipleBodiesWarningDuration;
				return;
			}
		}
	}
}

/**
 * Decrease the confidence of all head locations currently stored
 */
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

/**
 * Sets up the OSC client and server
 */
void UTheHoleOSCComponent::InitOSC()
{
	Client = UOSCManager::CreateOSCClient(BroadcastIPAdress, BroadcastPort, "TheHoleClient");

	Server = UOSCManager::CreateOSCServer(ReceiveIPAdress, ReceivePort, false, true, "TheHoleServer");
	Server->OnOscMessageReceived.AddDynamic(this, &UTheHoleOSCComponent::OnMessageReceived);
}

/**
 * Create the OSC messages that the client will send
 */
void UTheHoleOSCComponent::CreateMessages()
{
	HandshakeMessage.SetAddress(HandshakeAddress);
	HandshakeMessage = UOSCManager::AddInt32(HandshakeMessage, (int32)ReceivePort);

	UpdateMessage.SetAddress(UpdateAddress);
	UpdateMessage = UOSCManager::AddInt32(UpdateMessage, (int32)ReceivePort);
}

/**
 * Sends the handshake message at the start of its lifecycle.
 * Also sets up the timer that will send an update message periodically
 */
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

/**
 * Send the update message
 */
void UTheHoleOSCComponent::SendUpdate()
{
	Client->SendOSCMessage(UpdateMessage);
}

/**
 * Dispatches any incoming message.
 * Doesn't do anything if the message is not expected
 */
void UTheHoleOSCComponent::OnMessageReceived(const FOSCMessage& Message, const FString& IPAddress, int32 Port)
{
	FOSCAddress Address = Message.GetAddress();
	if (Address.Matches(SkeletonAddress))
	{
		OnSkeletonReceived(Message);
	}
	else if (Address.Matches(BlobAddress))
	{
		OnBlobReceived(Message);
	}
	else if (Address.Matches(MultipleBodiesAlertAddress))
	{
		OnMultipleBodiesDetected(Message);
	}
}

/**
 * Called whenever new skeleton data comes from OSC
 * 
 * @param Message An OSC message containing a skeleton's data
 */
void UTheHoleOSCComponent::OnSkeletonReceived(const FOSCMessage& Message)
{
	int32 id;
	float x, y, z, conf;
	UOSCManager::GetInt32(Message, 0, id);

	UOSCManager::GetFloat(Message, 1, x);
	UOSCManager::GetFloat(Message, 2, y);
	UOSCManager::GetFloat(Message, 3, z);
	UOSCManager::GetFloat(Message, 4, conf);

	SkeletonHeads.Add(id, FHead(FVector(x, -y, z), conf));
}

/**
 * Called whenever new blob data comes from OSC
 * 
 * @param Message An OSC message containing a blob's data
 */
void UTheHoleOSCComponent::OnBlobReceived(const FOSCMessage& Message)
{
	int32 id;
	float x, y, z;
	UOSCManager::GetInt32(Message, 0, id);

	UOSCManager::GetFloat(Message, 1, x);
	UOSCManager::GetFloat(Message, 2, y);
	UOSCManager::GetFloat(Message, 3, z);

	BlobHeads.Add(id, FHead(FVector(x, -y, z), 1.0f));
}

/**
 * Called whenever the multiple bodies message is received
 */
void UTheHoleOSCComponent::OnMultipleBodiesDetected(const FOSCMessage& Message)
{
	MultipleBodiesWarningTimer = MultipleBodiesWarningDuration;
}

