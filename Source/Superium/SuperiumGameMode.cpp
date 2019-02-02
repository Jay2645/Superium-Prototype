// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SuperiumGameMode.h"
#include "SuperiumHUD.h"
#include "SuperiumCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASuperiumGameMode::ASuperiumGameMode()
	: Super()
{
	// use our custom HUD class
	HUDClass = ASuperiumHUD::StaticClass();
}
