// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PStatComponent.generated.h"


UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VALHEIMMULTI_API UPStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPStatComponent();

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

	UPROPERTY(EditAnywhere, Category = "SurvPlayerStats")
	float HealthReduceValue;

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

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReduceHealth(float Value);
	bool ServerReduceHealth_Validate(float Value);
	void ServerReduceHealth_Implementation(float Value);

public:
	void AddHunger(float Value);
	void AddThirst(float Value);
	void AddHealth(float Value);
	void ReduceHunger(float Value);
	void ReduceThirst(float Value);
	void ReduceHealth(float Value);
	float GetHunger();
	float GetThirst();
	float GetHealth();

};
