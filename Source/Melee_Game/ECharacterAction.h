#pragma once
UENUM(BlueprintType)
enum class ECharacterAction : uint8 
{
	CA_NONE UMETA(DisplayName = "NONE"),
	CA_LIGHT UMETA(DisplayName = "LightAttack"),
	CA_UPPERCUT UMETA(DisplayName = "Uppercut"),
	CA_CHARGED UMETA(DisplayName = "Charged"),
	CA_HEAVYKICK UMETA(DisplayName = "HeavyKick"),
	CA_CHARGEDKICK UMETA(DisplayName = "ChargedKick")

};
