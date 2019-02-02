// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SuperiumGameMode.h"
#include "SuperiumHUD.h"
#include "SuperiumCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASuperiumGameMode::ASuperiumGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ASuperiumHUD::StaticClass();
}
