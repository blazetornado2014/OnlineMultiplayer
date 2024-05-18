// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemPickup.h"
#include "ValheimMultiCharacter.h"
#include "PStatComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AItemPickup::AItemPickup()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	RootComponent = MeshComponent;

	IncreaseAmt = 0.1f;

}

void AItemPickup::MultiCastDestroyActor_Implementation() 
{
	Destroy();
}

void AItemPickup::UseItem(AValheimMultiCharacter* Player)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (PickupType == EPickupType::EFood) 
		{
			UE_LOG(LogTemp, Warning, TEXT("USEITEM WORKS"));
			Player->PStatComponent->AddHunger(IncreaseAmt);
		}
		else if(PickupType == EPickupType::EWater)
		{
			UE_LOG(LogTemp, Warning, TEXT("USEITEM WORKS"));
			Player->PStatComponent->AddThirst(IncreaseAmt);
		}
		MultiCastDestroyActor();
	}
}

// Called when the game starts or when spawned
void AItemPickup::BeginPlay()
{
	Super::BeginPlay();
	SetReplicates(true);
	
}

bool AItemPickup::MultiCastDestroyActor_Validate()
{
	return true;
}



