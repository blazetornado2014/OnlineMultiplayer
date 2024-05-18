// Fill out your copyright notice in the Description page of Project Settings.


#include "PStatComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Engine.h"

// Sets default values for this component's properties
UPStatComponent::UPStatComponent()
{
	Hunger = 10.0f;
	HungryReduceValue = 0.01f;

	Thirst = 10.0f;
	ThirstReduceValue = 0.01f;

	Health = 100.0f;
	HealthReduceValue = 1.0f;
}


// Called when the game starts
void UPStatComponent::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated(true);

	GetWorld()->GetTimerManager().SetTimer(UtilTimerHandle, this, &UPStatComponent::ReduceUtilStats, 5.0f, true);
}

void UPStatComponent::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//replicate to everyone
	DOREPLIFETIME(UPStatComponent, Hunger);
	DOREPLIFETIME(UPStatComponent, Thirst);
	DOREPLIFETIME(UPStatComponent, Health);
}

void UPStatComponent::ReduceUtilStats()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		ReduceHunger(HungryReduceValue);
		ReduceThirst(ThirstReduceValue);
	}
}

bool UPStatComponent::ServerReduceHunger_Validate(float Value)
{
	return true;
}

void UPStatComponent::ServerReduceHunger_Implementation(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		ReduceHunger(Value);
	}
}

void UPStatComponent::AddHunger(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		Hunger += Value;
	}
}

void UPStatComponent::AddThirst(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		Thirst += Value;
	}
}

void UPStatComponent::AddHealth(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		Health += Value;
	}
}

void UPStatComponent::ReduceHunger(float Value)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerReduceHunger(Value);
	}
	else {
		Hunger -= Value;
	}
}

void UPStatComponent::ReduceThirst(float Value)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerReduceThirst(Value);
	}
	else {
		Thirst -= Value;
	}
}

void UPStatComponent::ReduceHealth(float Value)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		ServerReduceHealth(Value);
	}
	else if (GetOwnerRole() == ROLE_Authority)
	{
		Health -= Value;
	}
}

bool UPStatComponent::ServerReduceThirst_Validate(float Value)
{
	return true;
}

void UPStatComponent::ServerReduceThirst_Implementation(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		ReduceThirst(Value);
	}
}

bool UPStatComponent::ServerReduceHealth_Validate(float Value)
{
	return true;
}

void UPStatComponent::ServerReduceHealth_Implementation(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		ReduceHealth(Value);
	}
}

float UPStatComponent::GetHunger()
{
	return Hunger;
}

float UPStatComponent::GetThirst()
{
	return Thirst;
}

float UPStatComponent::GetHealth()
{
	return Health;
}