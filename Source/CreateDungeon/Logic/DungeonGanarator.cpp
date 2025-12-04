// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonGanarator.h"
#include "CreateDungeon/Room/RoomBase.h"
#include "CreateDungeon/Room/DungeonRoom1.h"
// Sets default values
ADungeonGanarator::ADungeonGanarator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADungeonGanarator::BeginPlay()
{
	Super::BeginPlay();
	SpawnStarterRooms();
	SpawnNextRoom();
}

// Called every frame
void ADungeonGanarator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ADungeonGanarator::SpawnStarterRooms()
{
	ADungeonRoom1* SpawnStartRoom = this->GetWorld()->SpawnActor<ADungeonRoom1>(StartRoom);
	if (SpawnStartRoom)
	{
		SpawnStartRoom->SetActorLocation(this->GetActorLocation());

		SpawnStartRoom->ExitPointsFolder->GetChildrenComponents(false, Exits);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("·ë ¸øÃ£À½"));
	}
}

void ADungeonGanarator::SpawnNextRoom()
{
	ARoomBase* LastestSpawnRoom = 
		this->GetWorld()->SpawnActor<ARoomBase>(RoomsToBeSpawned[rand()% RoomsToBeSpawned.Num()]);
	if (LastestSpawnRoom)
	{
		USceneComponent* SelectedExitPoint = Exits[rand() % Exits.Num()];

		LastestSpawnRoom->SetActorLocation(SelectedExitPoint->GetComponentLocation());
		LastestSpawnRoom->SetActorRotation(SelectedExitPoint->GetComponentRotation());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("·ë ¸øÃ£À½"));
	}
}
