// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entity/Weapon/RPGWeapon.h"
#include "SwordWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONPROJECT_API ASwordWeapon : public ARPGWeapon
{
	GENERATED_BODY()
	
public:
    ASwordWeapon();

    // 트레이스 시작/종료 (애님 노티파이 스테이트에서 호출)
    void StartWeaponTrace();
    void EndWeaponTrace();

    void ExecutionTrace();
protected:
    // 히트된 대상들에게 데미지 및 이벤트를 전달하는 내부 함수
    void ProcessHits(const TArray<FHitResult>& OutHits);

    // 무기 끝과 시작점을 알기 위한 소켓 이름
    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    FName TraceStartSocket = FName("TraceStart");

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    FName TraceEndSocket = FName("TraceEnd");

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    float TraceRadius = 15.f;

private:
    TArray<AActor*> HitActors; // 한 스윙에 중복 데미지 방지

    FVector LastStartPos;
    FVector LastEndPos;

    bool bIsTracing = false;
};
