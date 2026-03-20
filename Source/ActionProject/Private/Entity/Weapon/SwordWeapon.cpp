// Fill out your copyright notice in the Description page of Project Settings.
#include "Entity/Weapon/SwordWeapon.h"
#include "Entity/Weapon/RPGWeapon.h"
#include "Entity/Character/RPGCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

ASwordWeapon::ASwordWeapon()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ASwordWeapon::StartWeaponTrace()
{
    HitActors.Empty();
    bIsTracing = true;

    // 시작 지점의 소켓 위치를 저장 (다음 프레임과의 궤적을 잇기 위함)
    LastStartPos = WeaponMesh->GetSocketLocation(TraceStartSocket);
    LastEndPos = WeaponMesh->GetSocketLocation(TraceEndSocket);
}

void ASwordWeapon::EndWeaponTrace()
{
    bIsTracing = false;
    HitActors.Empty();
}

void ASwordWeapon::ExecutionTrace()
{
    if (!bIsTracing) return;

    // 1. 현재 프레임의 소켓 위치 확보
    FVector CurrentStart = WeaponMesh->GetSocketLocation(TraceStartSocket);
    FVector CurrentEnd = WeaponMesh->GetSocketLocation(TraceEndSocket);

    TArray<AActor*> ActorsToIgnore = { GetOwner(), this };
    TArray<FHitResult> OutHits;

    // 2. 면 판정 (Sweep): 이전 프레임의 칼날 위치에서 현재 칼날 위치까지 훑음
    // 여기서는 가장 흔히 쓰이는 '두 소켓 사이의 선분을 잇는 Sweep' 방식을 사용합니다.
    bool bHit = UKismetSystemLibrary::SphereTraceMulti(
        GetWorld(),
        CurrentStart,
        CurrentEnd,
        TraceRadius,
        UEngineTypes::ConvertToTraceType(ECC_Pawn), // 적 채널에 맞춰 수정 가능
        false,
        ActorsToIgnore,
        EDrawDebugTrace::ForDuration, // 디버깅용 (0.5초간 궤적 표시)
        OutHits,
        true
    );

    if (bHit)
    {
        ProcessHits(OutHits);
    }

    // 3. 다음 프레임을 위해 위치 갱신
    LastStartPos = CurrentStart;
    LastEndPos = CurrentEnd;
}

void ASwordWeapon::ProcessHits(const TArray<FHitResult>& OutHits)
{
    // 1. 무기의 소유자(캐릭터)를 가져옵니다.
    ARPGCharacter* OwnerChar = Cast<ARPGCharacter>(GetOwner());
    if (!OwnerChar) return;

    for (const FHitResult& Hit : OutHits)
    {
        AActor* Target = Hit.GetActor();

        // 2. 유효한 타겟이고 아직 안 맞은 경우만 처리
        if (Target && !HitActors.Contains(Target))
        {
            HitActors.Add(Target);

            // 3. 캐릭터에 이미 만들어둔 함수 호출!
            // 데미지 수치는 일단 임의로 넣거나, 무기 변수로 관리하면 좋습니다.
            OwnerChar->ApplyDamageToTarget(Target, WeaponData.BaseDamage, Hit);
        }
    }

}
