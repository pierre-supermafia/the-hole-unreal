// Fill out your copyright notice in the Description page of Project Settings.


#include "TheHoleOSCComponent.h"

const FOSCAddress UTheHoleOSCComponent::HandshakeAddress = FOSCAddress("/ks/request/handshake");
const FOSCAddress UTheHoleOSCComponent::UpdateAddress = FOSCAddress("/ks/request/update");
const FOSCAddress UTheHoleOSCComponent::SkeletonAddress= FOSCAddress("/ks/server/track/skeleton/head");
const FOSCAddress UTheHoleOSCComponent::BlobAddress = FOSCAddress("/ks/server/track/headblob");

const float UTheHoleOSCComponent::LowerConfidenceThreshold = 0.2f;

// Sets default values for this component's properties
UTheHoleOSCComponent::UTheHoleOSCComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	CreateMessages();
}


// Called when the game starts
void UTheHoleOSCComponent::BeginPlay()
{
	Super::BeginPlay();

	InitOSC();
	SendHandshake();	
}


// Called every frame
void UTheHoleOSCComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UTheHoleOSCComponent::GetHeadLocation(FVector& HeadLocation) const
{
	float TotalConfidence = 0;
	FVector TotalPosition = FVector(0, 0, 0);
	for (auto it = Heads.CreateConstIterator(); it; ++it)
	{
		TotalConfidence += it->Value.Confidence;
		TotalPosition += it->Value.Position;
	}

	if (TotalConfidence > LowerConfidenceThreshold)
	{
		HeadLocation = TotalPosition / TotalConfidence;
		return true;
	}
	else if (Blobs.Num() > 0)
	{
		TotalPosition = FVector(0, 0, 0);
		uint8 NumberOfBlobs = 0;
		for (auto it = Blobs.CreateConstIterator(); it; ++it)
		{
			TotalPosition += it->Value;
			NumberOfBlobs++;
		}
		HeadLocation = TotalPosition / NumberOfBlobs;
		return true;
	}
	else
	{
		return false;
	}
}

void UTheHoleOSCComponent::DecayConfidences()
{
	for (auto it = Heads.CreateIterator(); it; ++it)
	{
		it.Value().Confidence *= ConfidenceDecay;
	}
}


void UTheHoleOSCComponent::InitOSC()
{
	Client = UOSCManager::CreateOSCClient(BroadcastIPAdress, BroadcastPort, "TheHoleClient");

	Server = UOSCManager::CreateOSCServer(ReceiveIPAdress, ReceivePort, false, true, "TheHoleServer");
	Server->SetWhitelistClientsEnabled(false);

	Server->BindEventToOnOSCAddressPatternMatchesPath(SkeletonAddress, OnSkeletonReceivedDelegate);
	Server->BindEventToOnOSCAddressPatternMatchesPath(BlobAddress, OnBlobReceivedDelegate);

	OnSkeletonReceivedDelegate.BindUFunction(this, FName("OnSkeletonReceived"));
	OnBlobReceivedDelegate.BindUFunction(this, FName("OnBlobReceived"));
}

void UTheHoleOSCComponent::CreateMessages()
{
	HandshakeMessage.SetAddress(HandshakeAddress);
	UOSCManager::AddInt32(HandshakeMessage, ReceivePort);

	UpdateMessage.SetAddress(UpdateAddress);
	UOSCManager::AddInt32(UpdateMessage, ReceivePort);
}

void UTheHoleOSCComponent::SendHandshake()
{
	Client->SendOSCMessage(HandshakeMessage);
}

void UTheHoleOSCComponent::SendUpdate()
{
	Client->SendOSCMessage(UpdateMessage);
}

void UTheHoleOSCComponent::OnSkeletonReceived(const FOSCAddress& Address, FOSCMessage& Message)
{
	FOSCStream Stream;
	Message.GetPacket()->ReadData(Stream);

	uint8 id = Stream.ReadInt32();

	float x = Stream.ReadFloat();
	float y = Stream.ReadFloat();
	float z = Stream.ReadFloat();
	float conf = Stream.ReadFloat();

	Heads.Add(id, FHead(FVector(x, y, z), conf));
}

void UTheHoleOSCComponent::OnBlobReceived(const FOSCAddress& Address, FOSCMessage& Message)
{
	FOSCStream Stream;
	Message.GetPacket()->ReadData(Stream);

	uint8 id = Stream.ReadInt32();

	float x = Stream.ReadFloat();
	float y = Stream.ReadFloat();
	float z = Stream.ReadFloat();

	Blobs.Add(id, FVector(x, y, z));
}

