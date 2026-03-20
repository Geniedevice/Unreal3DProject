// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Character/RPGPlayerCharacter.h"
#include "Entity/Character/RPGCharacter.h"
#include "Entity/Weapon/RPGWeapon.h"
#include "Entity/Weapon/RPGItemTypes.h"
#include "Entity/Attribute/RPGGameplayTags.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"

ARPGPlayerCharacter::ARPGPlayerCharacter()
{
	// 플레이어 특화 설정이 필요하다면 여기서 수행
}

void ARPGPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 공격 중일 때만 회전 로직 실행
    if (bIsAttacking)
    {
        // 1. 현재 사용자의 입력 벡터를 가져옴 (WASD 입력값)
        FVector InputDirection = GetLastMovementInputVector();
        FRotator TargetRot;

        if (InputDirection.IsNearlyZero())
        {
            // 입력을 안 하고 있다면 카메라가 정면으로 회전
            TargetRot = FRotator(0.f, GetControlRotation().Yaw, 0.f);
        }
        else
        {
            // WASD 입력을 하고 있다면 그 입력 방향으로 회전
            TargetRot = InputDirection.Rotation();
        }

        // 2. 부드럽게 회전 (회전 속도는 10~20 사이 추천)
        FRotator NewRot = FMath::RInterpTo(GetActorRotation(), TargetRot, DeltaTime, 15.0f);
        SetActorRotation(NewRot);
    }
}

void ARPGPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Started: 버튼을 누른 시점
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ARPGPlayerCharacter::OnAttackInputPressed);

        // Completed: 버튼에서 손을 뗀 시점
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &ARPGPlayerCharacter::OnAttackInputReleased);

        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Canceled, this, &ARPGPlayerCharacter::OnAttackInputReleased);
    }
}

void ARPGPlayerCharacter::OnAttackInputPressed(const FInputActionValue& Value)
{
    if (!AbilitySystemComponent) return;

    int32 InputID = static_cast<int32>(ERPGAbilityInputID::Attack);

    // GAS 표준 입력 처리: 서버로 InputPressed 신호를 복제(Replication)합니다.

    AbilitySystemComponent->PressInputID(InputID);
}

void ARPGPlayerCharacter::OnAttackInputReleased(const FInputActionValue& Value)
{
    if (!AbilitySystemComponent) return;

    int32 InputID = static_cast<int32>(ERPGAbilityInputID::Attack);

    // 서버로 InputReleased 신호를 복제합니다.
    AbilitySystemComponent->ReleaseInputID(InputID);
}

