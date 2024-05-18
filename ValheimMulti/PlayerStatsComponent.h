// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStatsComponent.generated.h"


UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VALHEIMMULTI_API UPlayerStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerStatsComponent();

protected:

	UPROPERTY(Replicated)
	float Hunger;
	//Can potentially be scaled with difficulty
	UPROPERTY(EditAnywhere, Category = "SurvPlayerStats")
	float HungryReduceValue;

	UPROPERTY(Replicated)
	float Thirst;
	//Can potentially be scaled with difficulty
	UPROPERTY(EditAnywhere, Category = "SurvPlayerStats")
	float ThirstReduceValue;

	UPROPERTY(Replicated)
	float Health;

	FTimerHandle UtilTimerHandle;

	// Called when the game starts
	virtual void BeginPlay() override;

	void ReduceUtilStats();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReduceHunger(float Value);
	bool ServerReduceHunger_Validate(float Value);
	void ServerReduceHunger_Implementation(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReduceThirst(float Value);
	bool ServerReduceThirst_Validate(float Value);
	void ServerReduceThirst_Implementation(float Value);

public:	
	void ReduceHunger(float Value);
	void ReduceThirst(float Value);
	float GetHunger();
	float GetThirst();
		
};
