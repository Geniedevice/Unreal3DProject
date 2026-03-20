#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h" 
#include "GameplayAbilitySpec.h"
#include "Net/UnrealNetwork.h"
#include "RPGCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChangedDelegate, float, NewValue, float, MaxValue, float, Ratio);

UCLASS()
class ACTIONPROJECT_API ARPGCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ARPGCharacter();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Data")
	FGameplayTag MyCharacterTag;

	UPROPERTY()
	TObjectPtr<class URPGAttributeSet> AttributeSet;

	// 2. 이미지의 GE_DefaultAttributes_En... 파일을 할당할 변수입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Attributes")
	TSubclassOf<class UGameplayEffect> DefaultAttributeStat;

	/** 에디터에서 GE_Damage를 할당할 변수 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Combat")
	TSubclassOf<class UGameplayEffect> DamageEffectClass;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedDelegate OnHealthChangedDelegate;

	// 현재 부여된 공격 어빌리티의 핸들 (나중에 무기 해제 시 제거용)
	UPROPERTY(Replicated)
	FGameplayAbilitySpecHandle CurrentAttackAbilityHandle;

	// 무기 액터를 가져오기 위한 함수
	UFUNCTION(BlueprintCallable, Category = "GAS|Combat")
	class ARPGWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

	UFUNCTION(BlueprintCallable, Category = "GAS|Combat")
	void ApplyDamageToTarget(AActor* TargetActor, float DamageAmount, const FHitResult& HitResult);

	// -------------------------------------------------------
	// 초기화 로직 (이미지의 데이터를 처리하는 핵심 함수)
	virtual void InitializeAttributes();
	virtual void AddCharacterAbilities();

	virtual void OnHealthChanged(const struct FOnAttributeChangeData& Data);
	virtual void OnMaxHealthChanged(const struct FOnAttributeChangeData& Data);

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// --- 무기 시스템 ---
	// 클라이언트에서 호출 가능한 장착 함수
	UFUNCTION(BlueprintCallable, Category = "GAS|Combat")
	void EquipWeaponByName(FName WeaponRowName);

	// 실제 서버에서 실행될 장착 로직
	UFUNCTION(Server, Reliable)
	void ServerEquipWeaponByName(FName WeaponRowName);

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon, VisibleAnywhere, BlueprintReadOnly, Category = "GAS|Combat")
	TObjectPtr<class ARPGWeapon> CurrentWeapon;

	UFUNCTION()
	void OnRep_CurrentWeapon(ARPGWeapon* OldWeapon);

	void AttachWeaponToMesh(ARPGWeapon* Weapon, FName SocketName, FName RowName);

	// --- 어빌리티 관련 ---

	// 입력 처리를 위한 함수 (예: 마우스 왼쪽 클릭 시 호출)
	UFUNCTION(BlueprintCallable, Category = "GAS|Combat")
	void RequestAttack();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> StartupAbilities;

	virtual void GiveStartupAbilities();
	virtual void Die();
};