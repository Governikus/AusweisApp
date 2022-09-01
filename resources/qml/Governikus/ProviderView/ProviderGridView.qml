/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
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

	signal showDetails(var pModelItem)
	signal showAdditionalResults()

	GGridView {
		id: gridView

		property int columns: Math.floor(width / (ApplicationModel.scaleFactor * 400))
		property bool hasResults: gridView.count > 0 || ProviderCategoryFilterModel.additionalResultCount > 0
		property real spacing: Constants.component_spacing

		anchors {
			top: parent.top
			left: parent.left
			right: parent.right
			bottom: additionalResults.top
			topMargin: Math.floor(spacing / 2)
			leftMargin: Math.floor(spacing / 2)
			bottomMargin: Math.floor(spacing / 2)
		}

		cellWidth: Math.floor((width - spacing / 2) / columns)
		cellHeight: Math.floor(cellWidth * 0.84) // Set aspect ratio from ProviderCard
		displayMarginBeginning: spacing
		displayMarginEnd: additionalResults.height + spacing
		highlightFollowsCurrentItem: true
		activeFocusOnTab: true
		scrollBarTopPadding: spacing / 2
		scrollBarBottomPadding: spacing / 2

		model: ProviderCategoryFilterModel

		highlight: Item {
			FocusFrame {
				scope: gridView
				marginFactor: -0.75
				radius: Style.dimens.corner_radius
			}
		}

		delegate: Item {
			width: gridView.cellWidth
			height: gridView.cellHeight

			ProviderCard {
				anchors.fill: parent
				anchors.margins: Math.floor(gridView.spacing / 2)

				providerModelItem: model

				onShowDetailView: pModelItem => { baseItem.showDetails(pModelItem) }
			}
		}

		Connections {
			target: ProviderCategoryFilterModel
			function onFireCriteriaChanged() {
				gridView.currentIndex = 0
				gridView.contentY = gridView.originY
			}
		}
	}

	AdditionalResultsFooterItem {
		id: additionalResults

		visible: ProviderCategoryFilterModel.additionalResultCount > 0 && ProviderCategoryFilterModel.categories.length > 0 && ProviderCategoryFilterModel.categories.indexOf("all") === -1
		height: visible ? implicitHeight : 0
		width: gridView.columns * gridView.cellWidth
		anchors {
			bottom: parent.bottom
			left: parent.left
			right: parent.right
			leftMargin: Math.floor(gridView.spacing * 2)
			rightMargin: Math.floor(gridView.spacing * 2)
			topMargin: visible ? gridView.spacing : 0
			bottomMargin: visible ? gridView.spacing : 0
		}

		activeFocusOnTab: true

		onClicked: baseItem.showAdditionalResults()
	}

	GText {
		visible: ProviderCategoryFilterModel.rowCount === 0 && !additionalResults.visible

		anchors.centerIn: parent
		//: LABEL DESKTOP IOS_TABLET ANDROID_TABLET The text entered into the provider search field results in no matches
		text: qsTr("No results matching your search query found")
		textStyle: Style.text.normal
	}
}
