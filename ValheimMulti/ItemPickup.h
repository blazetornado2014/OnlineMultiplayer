// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ItemPickup.generated.h"


UENUM(BlueprintType)
enum class EPickupType : uint8
{
	EWater UMETA(DisplayName = "Water"),
	EFood UMETA(DisplayName = "Food")
	
};
UCLASS()
class VALHEIMMULTI_API AItemPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemPickup();

	void UseItem(class AValheimMultiCharacter* Player);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	float IncreaseAmt;

	UPROPERTY(EditAnywhere, Category = "Enums")
	EPickupType PickupType;

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastDestroyActor();
	bool MultiCastDestroyActor_Validate();
	void MultiCastDestroyActor_Implementation();

};
