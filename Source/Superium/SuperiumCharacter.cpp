// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SuperiumCharacter.h"
#include "SuperiumProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

ASuperiumCharacter::ASuperiumCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	PrimaryPower = NULL;
}

void ASuperiumCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	TArray<TSubclassOf<USuperpower>> powers = Powers.Array();
	for (TSubclassOf<USuperpower> power : powers)
	{
		ISuperInterface::Execute_AddSuperpower(this, power);
	}
}

TSet<TSubclassOf<USuperpower>> ASuperiumCharacter::GetSuperpowerClasses_Implementation() const
{
	return Powers;
}

TArray<USuperpower*> ASuperiumCharacter::GetSuperpowerComponents_Implementation() const
{
	return SuperpowerComponents;
}

USkeletalMeshComponent* ASuperiumCharacter::GetMesh_Implementation() const
{
	return Mesh1P;
}

void ASuperiumCharacter::PlayAnimation_Implementation(UAnimMontage* PowerAnimation, USkeletalMeshComponent* PowerMesh)
{
	// try and play an animation if specified
	if (PowerAnimation != NULL)
	{
		UAnimInstance* AnimInstance = NULL;
		if (PowerMesh == NULL)
		{
			// Get the animation object for the arms mesh
			AnimInstance = GetMesh()->GetAnimInstance();
		}
		else
		{
			AnimInstance = PowerMesh->GetAnimInstance();
		}
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(PowerAnimation, 1.f);
		}
	}
}

void ASuperiumCharacter::PlaySound_Implementation(USoundBase* PowerSound)
{
	// try and play the sound if specified
	if (PowerSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PowerSound, GetActorLocation());
	}
}

USceneComponent* ASuperiumCharacter::AddNewComponent_Implementation(TSubclassOf<USceneComponent> NewComponent, FTransform ComponentTransform, USceneComponent* AttachParent /*= NULL*/, FName AttachTo /*= NAME_None*/)
{
	USceneComponent* component = NewObject<USceneComponent>(this, NewComponent);
	if (component == NULL)
	{
		return NULL;
	}
	USceneComponent* attachComponent;
	if (AttachParent == NULL)
	{
		attachComponent = GetRootComponent();
	}
	else
	{
		attachComponent = AttachParent;
	}
	component->AttachToComponent(attachComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, AttachTo);
	component->AddLocalTransform(ComponentTransform);

	component->RegisterComponent();
	component->Activate(true);

	return component;
}

USuperpower* ASuperiumCharacter::AddSuperpower_Implementation(TSubclassOf<USuperpower> NewPower)
{
	if (NewPower == NULL)
	{
		return NULL;
	}
	Powers.Add(NewPower);
	
	USuperpower* superpower = NewObject<USuperpower>(this, NewPower);
	if (superpower == NULL)
	{
		return NULL;
	}

	superpower->RegisterComponent();
	superpower->Activate(true);

	SuperpowerComponents.Add(superpower);

	if (PrimaryPower == NULL)
	{
		PrimaryPower = superpower;
	}
	return superpower;
}

void ASuperiumCharacter::RemoveSuperpower_Implementation(int32 Index)
{
	if (!SuperpowerComponents.IsValidIndex(Index))
	{
		return;
	}
	UClass* powerClass = SuperpowerComponents[Index]->GetClass();
	Powers.Remove(powerClass);
	if (PrimaryPower == SuperpowerComponents[Index])
	{
		PrimaryPower = NULL;
	}
	SuperpowerComponents[Index]->DestroyComponent();
	SuperpowerComponents.RemoveAt(Index);

	// Reset primary power if we don't have one
	if (PrimaryPower == NULL && SuperpowerComponents.Num() > 0)
	{
		PrimaryPower = SuperpowerComponents[0];
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASuperiumCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASuperiumCharacter::OnPowerActivated);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASuperiumCharacter::OnPowerDeactivated);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ASuperiumCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ASuperiumCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASuperiumCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASuperiumCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASuperiumCharacter::LookUpAtRate);
}

void ASuperiumCharacter::OnPowerActivated()
{
	// try and fire a projectile
	if (PrimaryPower != NULL)
	{
		PrimaryPower->ActivatePower();
	}
}

void ASuperiumCharacter::OnPowerDeactivated()
{
	if (PrimaryPower != NULL)
	{
		PrimaryPower->DeactivatePower();
	}
}

void ASuperiumCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ASuperiumCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ASuperiumCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void ASuperiumCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void ASuperiumCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASuperiumCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ASuperiumCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASuperiumCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ASuperiumCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ASuperiumCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ASuperiumCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ASuperiumCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}
