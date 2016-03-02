// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealShooter.h"
#include "UnrealShooterDataSingleton.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"


UUnrealShooterDataSingleton::UUnrealShooterDataSingleton()
{
	UUnrealShooterDataSingleton::ParseJSON();
}

void UUnrealShooterDataSingleton::ParseJSON()
{
	FString JsonString;
	const FString fileName = "D:/Projects/UnrealProjects/UnrealShooter/Shared/UnrealShooterData.json";
	FFileHelper::LoadFileToString(JsonString, *fileName);
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		const TArray<TSharedPtr<FJsonValue>> SequencesJSON = JsonObject->GetArrayField(TEXT("sequences"));
		const TArray<TSharedPtr<FJsonValue>> WavesJSON = JsonObject->GetArrayField(TEXT("waves"));
		const TArray<TSharedPtr<FJsonValue>> TargetsJSON = JsonObject->GetArrayField(TEXT("targets"));
		const TArray<TSharedPtr<FJsonValue>> LocationsJSON = JsonObject->GetArrayField("locations");

		UUnrealShooterDataSingleton::ParseLocations(LocationsJSON);
		UUnrealShooterDataSingleton::ParseTargets(TargetsJSON);
		UUnrealShooterDataSingleton::ParseWaves(WavesJSON);
		UUnrealShooterDataSingleton::ParseSequences(SequencesJSON);
	}
}

void UUnrealShooterDataSingleton::ParseSequences(const TArray<TSharedPtr<FJsonValue>> &SequencesJSON)
{
	for (int32 i = 0; i != SequencesJSON.Num(); ++i)
	{
		FString sequenceName = SequencesJSON[i]->AsObject()->GetStringField(TEXT("sequenceName"));

		TArray<FTargetWave> waves;
		TArray<TSharedPtr<FJsonValue>> waveIDsSubJSON = SequencesJSON[i]->AsObject()->GetArrayField(TEXT("waveIDs"));
		for (int32 j = 0; j != waveIDsSubJSON.Num(); ++j)
		{
			int32 waveID = waveIDsSubJSON[j]->AsNumber();
			waves.Add(GetTargetWaveByWaveID(waveID));
		}
		Sequences.Emplace(FTargetSequenceStruct(sequenceName, waves));
	}
}

void UUnrealShooterDataSingleton::ParseWaves(const TArray<TSharedPtr<FJsonValue>> &WavesJSON)
{
	for (int32 i = 0; i != WavesJSON.Num(); ++i)
	{
		int32 waveID = WavesJSON[i]->AsObject()->GetIntegerField(TEXT("waveID"));
		int32 timeToLive = WavesJSON[i]->AsObject()->GetIntegerField(TEXT("timeToLive"));
		
		TArray<FRotatableTargetProperties> targets;
		TArray<TSharedPtr<FJsonValue>> targetIDsSubJSON = WavesJSON[i]->AsObject()->GetArrayField(TEXT("targetIDs"));
		for (int32 j = 0; j != targetIDsSubJSON.Num(); ++j)
		{
			int32 targetID = targetIDsSubJSON[j]->AsNumber();
			targets.Add(GetTargetPropertiesByTargetID(targetID));
		}
		Waves.Emplace(FTargetWave(waveID, targets, timeToLive));
	}
}

void UUnrealShooterDataSingleton::ParseTargets(const TArray<TSharedPtr<FJsonValue>> &TargetsJSON)
{
	for (int32 i = 0; i != TargetsJSON.Num(); ++i)
	{
		int32 targetID = TargetsJSON[i]->AsObject()->GetIntegerField(TEXT("targetID"));
		int32 initialLocationID = TargetsJSON[i]->AsObject()->GetIntegerField(TEXT("initialLocation"));
		int32 timeToLive = TargetsJSON[i]->AsObject()->GetIntegerField(TEXT("ttl"));
		FString targetType = TargetsJSON[i]->AsObject()->GetStringField(TEXT("targetType"));
		float speed = TargetsJSON[i]->AsObject()->GetNumberField(TEXT("speed"));
		bool isExplosive = TargetsJSON[i]->AsObject()->GetBoolField(TEXT("isExplosive"));
		
		FVector spawnPointLocation = GetTargetLocationByID(initialLocationID).Location;
		TArray<FVector> movementLocations;

		TArray<TSharedPtr<FJsonValue>> movementLocationsSubJSON = TargetsJSON[i]->AsObject()->GetArrayField(TEXT("movementLocations"));
		for (int32 j = 0; j != movementLocationsSubJSON.Num(); ++j)
		{
			int32 locationID = movementLocationsSubJSON[j]->AsNumber();
			movementLocations.Add(GetTargetLocationByID(locationID).Location);
		}
		Targets.Emplace(FRotatableTargetProperties(targetID, spawnPointLocation, timeToLive, UUnrealShooterDataSingleton::GetEnumByString(targetType), movementLocations, speed, isExplosive));
	}
}

void UUnrealShooterDataSingleton::ParseLocations(const TArray<TSharedPtr<FJsonValue>> &LocationsJSON)
{
	for (int32 i = 0; i != LocationsJSON.Num(); ++i)
	{
		int32 ID = LocationsJSON[i]->AsObject()->GetIntegerField(TEXT("locationID"));
		float x = LocationsJSON[i]->AsObject()->GetIntegerField(TEXT("x"));
		float y = LocationsJSON[i]->AsObject()->GetIntegerField(TEXT("y"));
		float z = LocationsJSON[i]->AsObject()->GetIntegerField(TEXT("z"));
		FVector targetLocation = { x, y, z };

		Locations.Emplace(FTargetLocation(ID, targetLocation));
	}
}

ETargetType UUnrealShooterDataSingleton::GetEnumByString(FString const & inString)
{
	if (inString == "SpecialTarget") return ETargetType::SpecialTarget;
	if (inString == "InnocentTarget") return ETargetType::InnocentTarget;
	if (inString == "FemaleTarget") return ETargetType::FemaleTarget;
	if (inString == "MaleTarget") return ETargetType::MaleTarget;
	return ETargetType::MaleTarget;
}

FTargetLocation UUnrealShooterDataSingleton::GetTargetLocationByID(int32 locationID)
{
	for (int32 i = 0; i != Locations.Num(); ++i)
	{
		int32 myLocationID = Locations[i].LocationID;
		if (myLocationID == locationID)
		{
			return Locations[i];
		}
	}
	return FTargetLocation();
}

FRotatableTargetProperties UUnrealShooterDataSingleton::GetTargetPropertiesByTargetID(int32 targetID)
{
	for (int32 i = 0; i != Targets.Num(); ++i)
	{
		int32 myTargetID = Targets[i].TargetID;
		if (targetID == myTargetID)
		{
			return Targets[i];
		}
	}
	return FRotatableTargetProperties();
}

FTargetWave UUnrealShooterDataSingleton::GetTargetWaveByWaveID(int32 waveID)
{
	for (int32 i = 0; i != Waves.Num(); ++i)
	{
		int32 myWaveID = Waves[i].WaveID;
		if (waveID == myWaveID)
		{
			return Waves[i];
		}
	}
	return FTargetWave();
}

FTargetSequenceStruct UUnrealShooterDataSingleton::GetSequenceBySequenceName(FString SequenceName)
{
	for (int32 i = 0; i != Sequences.Num(); ++i)
	{
		FString mySequence = Sequences[i].sequenceName;
		if (SequenceName == mySequence)
		{
			return Sequences[i];
		}
	}
	return FTargetSequenceStruct();
}