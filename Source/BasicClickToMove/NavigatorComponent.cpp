#include "NavigatorComponent.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UNavigatorComponent::UNavigatorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	Spline->ClearSplinePoints();
	DistanceToGoal = 0.0f;
	GoalDistanceThreshold = 100.0f;
	DistanceThreshold = 50.0f;
	DistanceBetweenPoints = 100.0f;
}

void UNavigatorComponent::Navigate(const TArray<FVector>& Points)
{
	Spline->ClearSplinePoints();
	for (FVector Point : Points)
	{
		Spline->AddSplineWorldPoint(Point);
		Goal = Point;
	}
	Length = Spline->GetSplineLength();
	Spline->Duration = Length;
	CurrentTime = 0.0f;
	NextPoint = Spline->GetLocationAtTime(CurrentTime, ESplineCoordinateSpace::World);
}

void UNavigatorComponent::Stop()
{
	Navigate(TArray<FVector>());
}

void UNavigatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Spline->GetNumberOfSplinePoints() == 0)
	{
		return;
	}

	const FVector ActorLocation = GetOwner()->GetActorLocation();
	DistanceToGoal = (Goal - ActorLocation).Size2D();
	if (DistanceToGoal <= GoalDistanceThreshold)
	{
		Spline->ClearSplinePoints();
		return;
	}

	const float DistanceToNextPoint = (NextPoint - ActorLocation).Size2D();
	if (DistanceToNextPoint <= DistanceThreshold)
	{
		CurrentTime += DistanceBetweenPoints;
		NextPoint = Spline->GetLocationAtTime(CurrentTime, ESplineCoordinateSpace::World);
	}

	const FVector Direction = (NextPoint - ActorLocation).GetSafeNormal2D();
	const FVector ActorDirection = GetOwner()->GetActorRotation().Vector().GetSafeNormal2D();
	const float Scale = FMath::Clamp(FVector::DotProduct(ActorDirection, Direction), 0.5f, 1.0f);
	Cast<APawn>(GetOwner())->AddMovementInput(Direction, Scale);
}


TArray<FVector> UNavigatorComponent::FindPathToLocation(FVector Location)
{
	TArray<FVector> Result;
	FNavPathSharedPtr Path;
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (NavSys && Character)
	{
		const ANavigationData* NavData = NavSys->GetNavDataForProps(Character->GetNavAgentPropertiesRef(), Character->GetNavAgentLocation());
		if (NavData)
		{
			const FSharedConstNavQueryFilter NavFilter = UNavigationQueryFilter::GetQueryFilter(*NavData, this, nullptr);
			FPathFindingQuery Query = FPathFindingQuery(*Character, *NavData, Character->GetNavAgentLocation(), Location, NavFilter);
			Query.SetAllowPartialPaths(true);

			FPathFindingResult PathResult = NavSys->FindPathSync(Query);
			if (PathResult.Result != ENavigationQueryResult::Error)
			{
				if (PathResult.IsSuccessful() && PathResult.Path.IsValid())
				{
					PathResult.Path->EnableRecalculationOnInvalidation(true);
					Path = PathResult.Path;
				}
			}
		}

		if (Path)
		{
			const TArray<FNavPathPoint>& Points = Path->GetPathPoints();
			for (int PointIndex = 0; PointIndex < Points.Num(); ++PointIndex)
			{
				Result.Add(Points[PointIndex].Location);
			}
		}
	}
	return Result;
}