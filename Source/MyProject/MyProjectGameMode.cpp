// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProjectGameMode.h"
#include "MyPlayerCharacter.h"

AMyProjectGameMode::AMyProjectGameMode()
{
	// Use our custom swimming character as the default pawn.
	// After creating BP_MyPlayerCharacter you can switch DefaultPawnClass to it in the editor.
	DefaultPawnClass = AMyPlayerCharacter::StaticClass();
}
