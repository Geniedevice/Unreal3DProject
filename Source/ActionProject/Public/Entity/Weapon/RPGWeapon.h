// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayAbilitySpec.h"
#include "Entity/Weapon/RPGItemTypes.h"
#include "RPGWeapon.generated.h"

UCLASS()
class ACTIONPROJECT_API ARPGWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	ARPGWeapon();

	// 무기를 캐릭터 손에 붙일 때 사용할 소켓 이름
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	FWeaponData WeaponData;

	// 어빌리티에서 "공격해!"라고 부를 핵심 함수
	// 검이면 여기서 충돌 체크를, 총이면 투사체를 쏩니다.
	virtual void Fire(AActor* InInstigator, const FGameplayEventData& Payload);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 무기 생성 후 서버에서 딱 한 번 호출하여 데이터를 채워줍니다.
	virtual void InitializeWeapon(FName InRowName);




protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

};
