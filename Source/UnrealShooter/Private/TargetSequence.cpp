// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealShooter.h"
#include "BasicButton.h"
#include "BasicSpawnPoint.h"
#include "SpecialTarget.h"
#include "RotatableTarget.h"
#include "Engine/DestructibleMesh.h"
#include "UnrealShooterDataSingleton.h"
#include "TargetSequence.h"

UTargetSequence::UTargetSequence()
{
	ConstructorHelpers::FObjectFinder<UClass> Target(TEXT("Blueprint'/Game/UnrealShooter/Blueprint/Target/TargetDummyBP.TargetDummyBP_C'"));
	TargetBP = Target.Object;

	ConstructorHelpers::FObjectFinder<UClass> specialTargetReference(TEXT("Blueprint'/Game/UnrealShooter/Blueprint/Target/SpecialTargetBP.SpecialTargetBP_C'"));
	SpecialTargetBP = specialTargetReference.Object;
}

void UTargetSequence::ApplyProperties(FString sqName, TArray<FTargetWave> SequenceWaves, UWorld* theWorld)
{
	this->sequenceName = sqName;
	this->Waves = SequenceWaves;
	this->World = theWorld;

	TargetsAvailable = 0;

	//event handler
	UUnrealShooterDataSingleton* DataInstance = Cast<UUnrealShooterDataSingleton>(GEngine->GameSingleton);
	DataInstance->OnTargetDestroyed.AddDynamic(this, &UTargetSequence::OnTargetDestroyedHandler);
}

void UTargetSequence::SpawnSpecialTarget()
{
	//Spawn special target
	ASpecialTarget* targetSpawned = World->SpawnActor<ASpecialTarget>(SpecialTargetBP);
	
	//default destination
	FVector Arr[] = { FVector(101.0f, 101.0f, 0.0f) };
	TArray<FVector> locations;
	locations.Append(Arr, ARRAY_COUNT(Arr));

	targetSpawned->ApplyProperties(FSpecialTargetProperties(FVector(100.0f, 100.0f, 0.0f), 0.0f, DEFAULT_SPECIAL_TARGET_POINTS, locations));
	TargetsAvailable++;
}

void UTargetSequence::OnTargetDestroyedHandler()
{
	//check if all the targets on this wave were destroyed
	TargetsAvailable--;
	//UE_LOG(LogTemp, Warning, TEXT("We still have: -- %d -- targets on the level"), TargetsAvailable);
	if (TargetsAvailable == 0)
	{
		//spawn the next wave
		PlayNextWave();
	}
}

void UTargetSequence::PlayNextWave()
{
	if (World)
	{
		_currentWave = GetNextWave();
		if (_currentWave.WaveID == -1.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayNextWave() - RELOAD TIME"));
			GEngine->AddOnScreenDebugMessage(-1, 3.0, FColor::Magenta, FString::FString("RELOAD TIME"));

			//this is because the wave is simply a "reload time" wave
			World->GetTimerManager().SetTimer(TimerHandle, this, &UTargetSequence::PlayNextWave, 3.0f, false);
		}
		else if (_currentWave.Targets.Num() > 0)
		{
			ARotatableTarget* spawnedTarget;
			for (auto& props : _currentWave.Targets)
			{
				spawnedTarget = World->SpawnActor<ARotatableTarget>(TargetBP);
				spawnedTarget->ApplyProperties(props, _currentWave.TimeToLive);
				TargetsAvailable++;
			}
		}
		else
		{
			//Sequence finished!
			ReactivatePlayWavesButton();
		}
	}
}

FTargetWave UTargetSequence::GetNextWave()
{
	//first wave not set
	if (_currentWave.WaveID != -1 && _currentWave.WaveID >= 0 && _currentWave.Targets.Num() == 0)
	{
		return Waves[0];
	}

	//for the rest of the waves
	for (int32 i = 0; i != Waves.Num(); ++i)
	{
		if (Waves[i].WaveID == _currentWave.WaveID)
		{
			if (i + 1 < Waves.Num())
			{
				return Waves[i + 1];
			}
		}
	}
	return FTargetWave();
}

void UTargetSequence::ReactivatePlayWavesButton()
{
	for (TActorIterator<ABasicButton> ActorItr(World); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ABasicButton *Button = *ActorItr;
		//UE_LOG(LogTemp, Warning, TEXT("ReactivatePlayWavesButton - %s"), *ActorItr->GetName());
		if (Button->GetName() == "SequenceButton")
		{
			Button->ActivateButton();
			break;
		}
	}
}


