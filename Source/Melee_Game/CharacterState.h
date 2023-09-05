#pragma once
UENUM(BlueprintType)
enum class ECharacterState : uint8 
{
	CS_NONE UMETA(DisplayName = "None"),
	CS_ATTACKING UMETA(DisplayName = "Attacking"),
	CS_DODGING UMETA(DisplayName = "Dodgind"),
	CS_DEATH UMETA(DisplayName = "Death"),
	CS_DISABLE UMETA(DisplayName = "Disable")
};
