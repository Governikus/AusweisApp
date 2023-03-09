/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0
import Governikus.Type.ApplicationModel 1.0

Item {
	id: baseItem
	signal showAdditionalResults
	signal showDetails(var pModelItem)

	GGridView {
		id: gridView

		property int columns: Math.floor(width / (ApplicationModel.scaleFactor * 400))
		property bool hasResults: gridView.count > 0 || ProviderCategoryFilterModel.additionalResultCount > 0
		property real spacing: Constants.component_spacing

		activeFocusOnTab: true
		cellHeight: Math.floor(cellWidth * 0.84) // Set aspect ratio from ProviderCard
		cellWidth: Math.floor((width - spacing / 2) / columns)
		displayMarginBeginning: spacing
		displayMarginEnd: additionalResults.height + spacing
		highlightFollowsCurrentItem: true
		model: ProviderCategoryFilterModel
		scrollBarBottomPadding: spacing / 2
		scrollBarTopPadding: spacing / 2

		delegate: Item {
			Accessible.name: card.Accessible.name
			Accessible.role: card.Accessible.role
			Keys.forwardTo: children
			height: gridView.cellHeight
			width: gridView.cellWidth

			ProviderCard {
				id: card
				anchors.fill: parent
				anchors.margins: Math.floor(gridView.spacing / 2)
				providerModelItem: model

				onShowDetailView: pModelItem => {
					baseItem.showDetails(pModelItem);
				}
			}
		}
		highlight: Item {
			FocusFrame {
				marginFactor: -0.75
				radius: Style.dimens.corner_radius
				scope: gridView
			}
		}

		anchors {
			bottom: additionalResults.top
			bottomMargin: Math.floor(spacing / 2)
			left: parent.left
			leftMargin: Math.floor(spacing / 2)
			right: parent.right
			top: parent.top
			topMargin: Math.floor(spacing / 2)
		}
		Connections {
			function onFireCriteriaChanged() {
				gridView.currentIndex = 0;
				gridView.contentY = gridView.originY;
			}

			target: ProviderCategoryFilterModel
		}
	}
	AdditionalResultsFooterItem {
		id: additionalResults
		activeFocusOnTab: true
		height: visible ? implicitHeight : 0
		visible: ProviderCategoryFilterModel.additionalResultCount > 0 && ProviderCategoryFilterModel.categories.length > 0 && ProviderCategoryFilterModel.categories.indexOf("all") === -1
		width: gridView.columns * gridView.cellWidth

		onClicked: baseItem.showAdditionalResults()

		anchors {
			bottom: parent.bottom
			bottomMargin: visible ? gridView.spacing : 0
			left: parent.left
			leftMargin: Math.floor(gridView.spacing * 2)
			right: parent.right
			rightMargin: Math.floor(gridView.spacing * 2)
			topMargin: visible ? gridView.spacing : 0
		}
	}
	GText {
		anchors.centerIn: parent
		//: LABEL DESKTOP IOS_TABLET ANDROID_TABLET The text entered into the provider search field results in no matches
		text: qsTr("No results matching your search query found")
		textStyle: Style.text.normal
		visible: ProviderCategoryFilterModel.rowCount === 0 && !additionalResults.visible
	}
}
