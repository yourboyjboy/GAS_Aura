// Copyright Jordan McWilliams


#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Interaction/CombatInterface.h"

UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
	bAutoActivate = false;
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
}

void UDebuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!GetOwner()->HasAuthority()) return;
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner());
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (ASC)
	{
		ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddLambda(
		[this](const FGameplayTag CallbackTag, const int32 NewCount)
		{
			if (NewCount > 0) { Activate(); } else { Deactivate(); }
		});
	} else if (CombatInterface)
	{
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* InASC)
		{
			InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddLambda(
		[this](const FGameplayTag CallbackTag, const int32 NewCount)
			{
				if (NewCount > 0) { Activate(); } else { Deactivate(); }
			});
		});
	}
	if (CombatInterface)
	{
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UDebuffNiagaraComponent::OnOwnerDeath);
	}
}

void UDebuffNiagaraComponent::OnOwnerDeath(AActor* Character) { Deactivate(); }
