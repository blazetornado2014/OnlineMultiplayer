// Copyright Epic Games, Inc. All Rights Reserved.
#include "PStatComponent.h"
#include "LineTrace.h"
#include "ItemPickup.h"
#include "Engine/DamageEvents.h"

#include "ValheimMultiCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AValheimMultiCharacter

AValheimMultiCharacter::AValheimMultiCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	PStatComponent = CreateDefaultSubobject<UPStatComponent>("PlayerStatComponent");
	LineTraceComponent = CreateDefaultSubobject<ULineTrace>("LineTrace");

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}


void AValheimMultiCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// Input

void AValheimMultiCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AValheimMultiCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AValheimMultiCharacter::Look);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AValheimMultiCharacter::Interact);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AValheimMultiCharacter::Attack);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AValheimMultiCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AValheimMultiCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}

}

void AValheimMultiCharacter::Interact(const FInputActionValue& Value)
{
	FVector Start = GetMesh()->GetBoneLocation(FName("head"));
	FVector End = Start + FollowCamera->GetForwardVector() * 142.0f;
	FHitResult HitResult = LineTraceComponent->LineTraceSingle(Start, End, true);

	if(AActor* Actor = HitResult.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *Actor->GetName());
		if (AItemPickup* Pickup = Cast<AItemPickup>(Actor))
		{
			UE_LOG(LogTemp, Warning, TEXT("ACTOR CAN BE PICKED UP"));
			ServerInteract();
		}
	}
}


bool AValheimMultiCharacter::ServerInteract_Validate()
{
	return true;
}

void AValheimMultiCharacter::ServerInteract_Implementation()
{
	if (GetLocalRole() == ROLE_Authority) 
	{
		FVector Start = GetMesh()->GetBoneLocation(FName("head"));
		FVector End = Start + FollowCamera->GetForwardVector() * 142.0f;
		FHitResult HitResult = LineTraceComponent->LineTraceSingle(Start, End, true);

		if (AActor* Actor = HitResult.GetActor())
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *Actor->GetName());
			if (AItemPickup* Pickup = Cast<AItemPickup>(Actor))
			{
				Pickup->UseItem(this);
			}
		}
	}
}

void AValheimMultiCharacter::Attack()
{
	FVector Start = GetMesh()->GetBoneLocation(FName("head"));
	FVector End = Start + FollowCamera->GetForwardVector() * 1420.0f;
	FHitResult HitResult = LineTraceComponent->LineTraceSingle(Start, End, true);

	if (AActor* Actor = HitResult.GetActor())
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *Actor->GetName());
		if (AValheimMultiCharacter* Player = Cast<AValheimMultiCharacter>(Actor))
		{
			UE_LOG(LogTemp, Warning, TEXT("ACTOR CAN BE PICKED UP"));
			ServerAttack();
		}
	}
}

bool AValheimMultiCharacter::ServerAttack_Validate()
{
	return true;
}

void AValheimMultiCharacter::ServerAttack_Implementation()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FVector Start = GetMesh()->GetBoneLocation(FName("head"));
		FVector End = Start + FollowCamera->GetForwardVector() * 1420.0f;
		FHitResult HitResult = LineTraceComponent->LineTraceSingle(Start, End);

		if (AActor* Actor = HitResult.GetActor())
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *Actor->GetName());
			if (AValheimMultiCharacter* Player = Cast<AValheimMultiCharacter>(Actor))
			{
				float TestDamage = 20.0f;
				Player->TakeDamage(TestDamage, FDamageEvent(), GetController(), this);
			}
		}
	}
}

float AValheimMultiCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (GetLocalRole() < ROLE_Authority || PStatComponent->GetHealth() <= 0.0f)
	{
		return 0.0f;
	}
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage > 0.0f)
	{
		PStatComponent->ReduceHealth(ActualDamage);
	}
	return ActualDamage;
}

//TESTING DELETE BEFORE PUSH
FString AValheimMultiCharacter::ReturnPlayerStats()
{
	FString RetString = "Hunger: " + FString::SanitizeFloat(PStatComponent->GetHunger())
		+ " Thirst: " + FString::SanitizeFloat(PStatComponent->GetThirst()) 
			+ " Health: " + FString::SanitizeFloat(PStatComponent->GetHealth());

	return RetString;
}

