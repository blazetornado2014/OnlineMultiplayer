// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatsComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Engine.h"

// Sets default values for this component's properties
UPlayerStatsComponent::UPlayerStatsComponent()
{
	Hunger = 10.0f;
	HungryReduceValue = 0.01f;

	Thirst = 10.0f;
	ThirstReduceValue = 0.01f;

	Health = 100.0f;
}


// Called when the game starts
void UPlayerStatsComponent::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated(true);

	GetWorld()->GetTimerManager().SetTimer(UtilTimerHandle, this, &UPlayerStatsComponent::ReduceUtilStats, 5.0f, true);
}

void UPlayerStatsComponent::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//replicate to everyone
	DOREPLIFETIME(UPlayerStatsComponent, Hunger);
	DOREPLIFETIME(UPlayerStatsComponent, Thirst);
	DOREPLIFETIME(UPlayerStatsComponent, Health);
}

void UPlayerStatsComponent::ReduceUtilStats()
{
	if (GetOwnerRole() == ROLE_Authority) 
	{
		ReduceHunger(HungryReduceValue);
		ReduceThirst(ThirstReduceValue);
	}
}

bool UPlayerStatsComponent::ServerReduceHunger_Validate(float Value)
{
	return true;
}

void UPlayerStatsComponent::ServerReduceHunger_Implementation(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		ReduceHunger(Value);
	}
}

void UPlayerStatsComponent::ReduceHunger(float Value)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerReduceHunger(Value);
	}
	else {
		Hunger -= Value;
	}
}

void UPlayerStatsComponent::ReduceThirst(float Value)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerReduceThirst(Value);
	}
	else {
		Thirst -= Value;
	}
}

bool UPlayerStatsComponent::ServerReduceThirst_Validate(float Value)
{
	return true;
}

void UPlayerStatsComponent::ServerReduceThirst_Implementation(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		ReduceThirst(Value);
	}
}

float UPlayerStatsComponent::GetHunger()
{
	return Hunger;
}

float UPlayerStatsComponent::GetThirst()
{
	return Thirst;
}
