// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonGanarator.generated.h"
class ADungeonRoom1;
class ARoomBase;
class AClosingWall;

UCLASS()
class CREATEDUNGEON_API ADungeonGanarator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADungeonGanarator();

	//던전의 모든 방이 생성 끝나면 뚫린 벽 막아주는 클래스
	UPROPERTY(EditAnywhere, Category = "UnusedExits")
	TSubclassOf<AClosingWall> ClosingWall;

	//시작방
	UPROPERTY(EditAnywhere, Category = "Rooms")
	TSubclassOf<ADungeonRoom1> StartRoom;

	//생성된 방들
	UPROPERTY(EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<ARoomBase>> RoomsToBeSpawned;

	//방 사이 복도
	UPROPERTY(EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<ARoomBase>> CorridorRooms;

	//던전 크기
	UPROPERTY(EditAnywhere, Category = "DungeonInfo")
	int32 RoomAmount;

	//보스방
	UPROPERTY(EditAnywhere, Category = "Boss")
	TSubclassOf<ARoomBase> BossRoomClass; // 연결할 보스 방 클래스

	//출구(다음방 통로)
	TArray<USceneComponent*> Exits;

	//시작방 생성하는 함수
	void SpawnStarterRooms();

	//다음 방 생성하는 함수 RoomAmount남아있으면 재귀호출로 계속 생성함
	void SpawnNextRoom();

	//방이나 복도 겹치면 지워버리고 다시 SpawnNextRoom호출해서 새로운 방 만드는 함수
	void RemoveOverlappingRooms();

	//모든 일반 방이 끝나고 나머지 작업 시작할댸 호출하는 함수
	void AfterEndedSpawnNomalRooms();

	//1. 보물방으로 사용할 방 선택
	void SelectedSpecialRoom();

	//보스방 생성
	bool SpawnBossRoom();

	//모든 작업 끝나고 빈 통로 닫기
	void ClosingUnuusedWall();

	//보스방 생성에 문제가 있을때 문제 없을때까지 리셋
	void ResetDungeon();

private:
	//ResetDungeon에 쓸 변수들
	//벽이랑 복도 저장하는 배열(지우기 위함)
	UPROPERTY()
	TArray<AActor*> GeneratedActors;

	//RoomAmount를 실시간으로 감소시키면서 방을 생성하기 때문에 RoomAmount의 초기값을 저장해야 함, 그ㅡ때쓰는 변수
	int32 InitialRoomAmount;

	//현재 리셋 카운트
	int32 CurrentResetCount = 0;

	//최대 리셋 카운트(나중에 지울 예정)
	const int32 MaxResetLimit = 10;

protected:
	//마지막으로 생성된 방
	ARoomBase* LastestSpawnRoom;

	//방이 생성이 가능한지 체크하는 변수
	bool bCanSpawn = false;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
