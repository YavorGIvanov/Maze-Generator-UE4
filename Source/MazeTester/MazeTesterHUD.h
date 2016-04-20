// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "MazeTesterHUD.generated.h"

UCLASS()
class AMazeTesterHUD : public AHUD
{
	GENERATED_BODY()

public:
	AMazeTesterHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

