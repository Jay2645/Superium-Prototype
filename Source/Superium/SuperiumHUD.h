// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SuperiumHUD.generated.h"

UCLASS()
class ASuperiumHUD : public AHUD
{
	GENERATED_BODY()

public:
	ASuperiumHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UTexture2D* CrosshairTex;

};

