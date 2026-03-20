// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entity/Character/RPGCharacter.h"
#include "InputActionValue.h"
#include "RPGPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONPROJECT_API ARPGPlayerCharacter : public ARPGCharacter
{
	GENERATED_BODY()
	
public:
	ARPGPlayerCharacter();

	// 공격 중인지 여부 (Ability에서 제어)
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bIsAttacking = false;

	// 회전 속도 (기본값 15.0f 추천)
	UPROPERTY(EditAnywhere, Category = "Combat")
	float RotationSpeed = 15.f;

protected:
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** 에디터에서 할당할 공격용 Input Action (좌클릭) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> AttackAction;

	void OnAttackInputPressed(const FInputActionValue& Value);

	// 공격 입력 종료 (Release)
	void OnAttackInputReleased(const FInputActionValue& Value);
};
