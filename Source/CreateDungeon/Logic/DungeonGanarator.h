// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonGanarator.generated.h"
class ADungeonRoom1;
class ARoomBase;


UCLASS()
class CREATEDUNGEON_API ADungeonGanarator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADungeonGanarator();

	UPROPERTY(EditAnywhere, Category = "Rooms")
	TSubclassOf<ADungeonRoom1> StartRoom;

	UPROPERTY(EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<ARoomBase>> RoomsToBeSpawned;

	UPROPERTY(EditAnywhere, Category = "DungeonInfo")
	int32 RoomAmount;

	TArray<USceneComponent*> Exits;

	void SpawnStarterRooms();

	void SpawnNextRoom();

	void RemoveOverlappingRooms();
protected:
	ARoomBase* LastestSpawnRoom;

	bool bCanSpawn = false;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
