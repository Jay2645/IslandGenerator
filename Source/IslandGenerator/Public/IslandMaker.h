// Copyright 2018 Jay Stevens

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "DelaunayHelper.h"

#include "IslandMap.h"
#include "IslandMaker.generated.h"

USTRUCT(BlueprintType)
struct ISLANDGENERATOR_API FIslandRegion : public FPointIndex
{
	GENERATED_BODY()

public:
	bool bIsCoast;
	bool bIsWater;
	bool bIsOcean;
	float Elevation;
	int32 WaterDistance;
	float Moisture;
	float Temperature;
	FGameplayTag Biome;
	FColor DebugColor;

public:
	FIslandRegion()
	{
		Value = INVALID_DELAUNAY_INDEX;
		bIsCoast = false;
		bIsOcean = false;
		bIsWater = false;
		Elevation = 0.0f;
		WaterDistance = -1;
		Moisture = 0.0f;
		Temperature = 0.0f;
		Biome = FGameplayTag::EmptyTag;
		DebugColor = FColor::Magenta;
	}
};

USTRUCT(BlueprintType)
struct ISLANDGENERATOR_API FIslandTriangle : public FTriangleIndex
{
	GENERATED_BODY()

public:
	int32 CoastDistance;
	float Elevation;
	FSideIndex Downslope;
	bool bIsRiver;
	bool bIsSpring;

public:
	FIslandTriangle()
	{
		Value = INVALID_DELAUNAY_INDEX;

		Elevation = 0.0f;
		CoastDistance = -1;
		bIsRiver = false;
		bIsSpring = false;
	}
};

/**
 * 
 */
UCLASS()
class ISLANDGENERATOR_API AIslandMaker : public AIslandMap
{
	GENERATED_BODY()

protected:
	TArray<FIslandRegion> Regions;
	TArray<FIslandTriangle> Triangles;

protected:
	virtual void OnIslandGenComplete_Implementation() override;
};
