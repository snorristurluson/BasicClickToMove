#include "PathVisualizerComponent.h"

UPathVisualizerComponent::UPathVisualizerComponent() :
	DistanceBetweenPoints(100.0f)
{
	Path = CreateDefaultSubobject<USplineComponent>(TEXT("Path"));
	Mesh = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("Mesh"));
	Mesh->NumCustomDataFloats = 1;
}

void UPathVisualizerComponent::SetupSpline(const TArray<FVector>& Waypoints)
{
	Path->ClearSplinePoints(false);
	float Index = 0.0f;
	for (FVector Point : Waypoints)
	{
		FSplinePoint SplinePoint(Index, Point, ESplinePointType::CurveClamped);
		Path->AddPoint(SplinePoint, false);
		Index += 1.0f;
	}
	Path->UpdateSpline();
}

void UPathVisualizerComponent::SetupMesh()
{
	Mesh->ClearInstances();
	float Length = Path->GetSplineLength();

	int NumPoints = FMath::RoundToInt(Length / DistanceBetweenPoints);

	float D = DistanceBetweenPoints;
	for (int PointIndex = 0; PointIndex < NumPoints; ++PointIndex)
	{
		FVector Location = Path->GetLocationAtDistanceAlongSpline(D, ESplineCoordinateSpace::World);
		FTransform Transform(Location);
		Mesh->AddInstance(Transform);
		Mesh->SetCustomDataValue(PointIndex, 0, D / Length);
		D += DistanceBetweenPoints;
	}
}

void UPathVisualizerComponent::SetWaypoints(const TArray<FVector>& Waypoints)
{
	SetupSpline(Waypoints);
	SetupMesh();
}
