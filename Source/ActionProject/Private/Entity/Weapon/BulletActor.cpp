#include "Entity/Weapon/BulletActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Entity/Attribute/RPGGameplayTags.h"
#include <Entity/Character/RPGPlayerCharacter.h>

ABulletActor::ABulletActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // 1. 네트워크 복제 설정 (서버에서 스폰 시 클라이언트에도 보이게 함)
    bReplicates = true;
    SetReplicateMovement(true);

    // 2. 충돌체 설정 (최상위 루트)
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(5.0f);

    // [중요] 충돌 프로필 및 이벤트 설정
    CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
    CollisionComp->SetGenerateOverlapEvents(true);
    CollisionComp->SetNotifyRigidBodyCollision(true); // "Simulation Generates Hit Events" 체크와 동일
    CollisionComp->BodyInstance.bUseCCD = true;       // 빠른 물체 뚫림 방지 (Continuous Collision Detection)

    RootComponent = CollisionComp;

    // 3. 메시 컴포넌트 (시각용)
    BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
    BulletMesh->SetupAttachment(RootComponent);
    BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 메시는 충돌 계산 안함

    // 4. 발사체 이동 컴포넌트
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = 5000.f;
    ProjectileMovement->MaxSpeed = 5000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->ProjectileGravityScale = 0.0f; // 직선 비행

    // 수명 설정 (3초 뒤 자동 소멸)
    InitialLifeSpan = 3.0f;

    // 5. [핵심 수정] 이벤트 바인딩은 반드시 충돌을 담당하는 컴포넌트에!
    CollisionComp->OnComponentHit.AddDynamic(this, &ABulletActor::OnHit);
}

void ABulletActor::InitializeProjectile(const FGameplayEffectSpecHandle& InSpecHandle)
{
    DamageSpecHandle = InSpecHandle;
}

void ABulletActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor == nullptr || OtherActor == GetInstigator()) return;

    ARPGCharacter* Shooter = Cast<ARPGCharacter>(GetInstigator());

    if (Shooter && OtherActor && DamageSpecHandle.IsValid())
    {
        // 1. SpecHandle에서 실제 Spec 데이터 포인터를 가져옵니다.
        FGameplayEffectSpec* Spec = DamageSpecHandle.Data.Get();

        // 2. SetByCaller로 저장했던 데미지 값을 다시 꺼내옵니다.
        // FRPGGameplayTags::Get().Data_Damage 태그에 저장된 값을 찾습니다.
        float ExtractedDamage = Spec->GetSetByCallerMagnitude(FRPGGameplayTags::Get().Data_Damage, true, 0.0f);

        // 3. 기존 인자(float)를 사용하는 함수 그대로 호출!
        Shooter->ApplyDamageToTarget(OtherActor, ExtractedDamage, Hit);

        Destroy();
    }
}