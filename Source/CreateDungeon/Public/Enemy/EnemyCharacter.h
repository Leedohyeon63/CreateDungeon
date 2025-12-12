// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Data/EnumBase.h"
#include "Intetface/EnemyInterface.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class CREATEDUNGEON_API AEnemyCharacter : public ACharacter, public IEnemyInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
    //착용할 무기 블루프린트에서 할당함
    UPROPERTY(EditAnywhere, Category = "Combat")
    TSubclassOf<AActor> WeaponClass;

    // 현재 무기
    UPROPERTY(VisibleAnywhere, Category = "Combat")
    AActor* CurrentWeapon;

    //공격 애니메이션
    UPROPERTY(EditAnywhere, Category = "Combat")
    UAnimMontage* AttackMontage;

    //소켓 이름(나중에 통일할 예정)
    UPROPERTY(EditAnywhere, Category = "Combat")
    FName WeaponSocketName = TEXT("RHandSoket");

    //상태 확인용 변수 (BT 데코레이터용)
    UPROPERTY(BlueprintReadWrite, Category = "Combat")
    bool bIsWieldingWeapon;

    //무기 장착 함수 나중에 WieldWeapon으로 수정해야 함
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void WieldWeapon();

    //공격, 현재는 몽타주만 재생함
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void DefaultAttack();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool PlayerFocus(AActor* TargetActor, float DeltaTime, float TurnSpeed);

    //이동속도 설정
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void UpdateMovementSpeed(float NewSpeed);


    //인터페이스 함수 오버라이드, EnumBase에 있는 열거형에 따라 상태별 속도 설정
    virtual float SetMovementSpeed_Implementation(EEnemySpeed State) override;
};
