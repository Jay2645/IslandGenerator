// Copyright 2018 Jay Stevens

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "DelaunayHelper.h"

#include "IslandMapMesh.h"

#include "Rivers/NamedIslandRivers.h"
#include "RiverSplineComponent.h"

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
class ISLANDGENERATOR_API AIslandMaker : public AIslandMapMesh
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FIslandRegion> Regions;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FIslandTriangle> Triangles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "River")
	TArray<URiverSplineComponent*> RiverSplines;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "River")
	float RiverErosionFactor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "River")
	UMaterialInterface* RiverInterface;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "River")
	TMap<FName, UTexture*> RiverTextures;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "River")
	float PointScale;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "River")
	UStaticMesh* RiverMesh;

public:
	AIslandMaker();

protected:
	virtual void OnRiverGenerationComplete_Implementation() override;
	virtual URiverSplineComponent* CreateRiverSpine(UNamedRiver* River, TArray<FVector> SplinePositions);
	virtual TArray<FVector> ProcessRiver(UNamedRiver* River, int32 MaxFlow);
	virtual void OnIslandGenComplete_Implementation() override;
};
