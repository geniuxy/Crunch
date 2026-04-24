// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Item/ItemTreeWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Widgets/Tree/SplineWidget.h"
#include "Widgets/Tree/TreeNodeInterface.h"

void UItemTreeWidget::DrawStream(
	bool bUpperStream,
	const ITreeNodeInterface* StartingNodeInterface,
	UUserWidget* StartingNodeWidget,
	UCanvasPanelSlot* StartingNodeSlot,
	int StartingNodeDepth,
	float& NextLeafXPosition,
	TArray<UCanvasPanelSlot*>& OutStreamSlots)
{
	TArray<const ITreeNodeInterface*> NextTreeNodeInterfaces =
		bUpperStream ? StartingNodeInterface->GetInputs() : StartingNodeInterface->GetOutputs();
	float StartingNodeYPos = (NodeSize.Y + NodeGap.Y) * StartingNodeDepth * (bUpperStream ? -1 : 1);

	if (NextTreeNodeInterfaces.Num() == 0)
	{
		StartingNodeSlot->SetPosition(FVector2D(NextLeafXPosition, StartingNodeYPos));
		NextLeafXPosition += NodeSize.X + NodeGap.X;
		return;
	}

	float NextNodeXPosSum = 0;
	for (const ITreeNodeInterface* NextTreeNodeInterface : NextTreeNodeInterfaces)
	{
		UCanvasPanelSlot* NextWidgetSlot = nullptr;
		UUserWidget* NextWidget = CreateWidgetForNode(NextTreeNodeInterface, NextWidgetSlot);
		OutStreamSlots.Add(NextWidgetSlot);
		if (bUpperStream)
		{
			CreateConnection(NextWidget, StartingNodeWidget);
		}
		else
		{
			CreateConnection(StartingNodeWidget, NextWidget);
		}

		DrawStream(bUpperStream, NextTreeNodeInterface, NextWidget, NextWidgetSlot, StartingNodeDepth + 1,
		           NextLeafXPosition, OutStreamSlots);
		NextNodeXPosSum += NextWidgetSlot->GetPosition().X;
	}

	float StartingNodeXPos = NextNodeXPosSum / NextTreeNodeInterfaces.Num();
	StartingNodeSlot->SetPosition(FVector2D(StartingNodeXPos, StartingNodeYPos));
}

void UItemTreeWidget::ClearTree()
{
	RootPanel->ClearChildren();
}

UUserWidget* UItemTreeWidget::CreateWidgetForNode(const ITreeNodeInterface* Node, UCanvasPanelSlot* OutCanvasSlot)
{
	if (!Node) return nullptr;

	UUserWidget* NodeWidget = Node->GetWidget();
	OutCanvasSlot = RootPanel->AddChildToCanvas(NodeWidget);
	if (OutCanvasSlot)
	{
		OutCanvasSlot->SetSize(NodeSize);
		OutCanvasSlot->SetAnchors(FAnchors(0.5f));
		OutCanvasSlot->SetAlignment(FVector2D(0.5f));
		OutCanvasSlot->SetZOrder(1);
	}

	return NodeWidget;
}

void UItemTreeWidget::CreateConnection(const UUserWidget* From, const UUserWidget* To)
{
	if (!From || !To) return;

	USplineWidget* Connection = CreateWidget<USplineWidget>(GetOwningPlayer());
	UCanvasPanelSlot* ConnectionPanelSlot = RootPanel->AddChildToCanvas(Connection);
	if (ConnectionPanelSlot)
	{
		ConnectionPanelSlot->SetAnchors(FAnchors(0.5f));
		ConnectionPanelSlot->SetAlignment(FVector2D(0.5f));
		ConnectionPanelSlot->SetPosition(FVector2D::Zero());
		ConnectionPanelSlot->SetZOrder(0);
	}
	Connection->SetupSpline(
		From, To, SourcePortLocalPos, DestinationPortLocalPos, SourcePortDirection, DestinationPortDirection
	);
	Connection->SetSplineStyle(ConnectionColor, ConnectionThickness);
}
