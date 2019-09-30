/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0
import Governikus.Type.ApplicationModel 1.0

SectionPage {
	id: baseItem

	signal showDetailView(var pModelItem)

	Accessible.name: qsTr("Provider view") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the provider view of the AusweisApp2.") + SettingsModel.translationTrigger

	Component.onCompleted: {
		ProviderCategoryFilterModel.setCategorySelection("all")
		ProviderCategoryFilterModel.searchString = ""
		tabbedPane.currentIndex = 0
	}

	TabbedPane {
		id: tabbedPane

		anchors.fill: parent
		anchors.margins: Constants.pane_padding

		sectionsModel: [
			{
				//: LABEL DESKTOP_QML
				categoryName: qsTr("All provider") + SettingsModel.translationTrigger,
				category: "all"
			},
			{
				//: LABEL DESKTOP_QML
				categoryName: qsTr("Citizen services") + SettingsModel.translationTrigger,
				category: "citizen"
			},
			{
				//: LABEL DESKTOP_QML
				categoryName: qsTr("Financials") + SettingsModel.translationTrigger,
				category: "finance"
			},
			{
				//: LABEL DESKTOP_QML
				categoryName: qsTr("Insurances") + SettingsModel.translationTrigger,
				category: "insurance"
			},
			{
				//: LABEL DESKTOP_QML
				categoryName: qsTr("Other services") + SettingsModel.translationTrigger,
				category: "other"
			}
		]

		sectionDelegate: TabbedPaneDelegateIconAndText {
			sectionName: model.modelData.categoryName
			iconPath: Category.imageSource(model.modelData.category)
		}
		contentDelegate: content
		contentPadding: 0
		onCurrentItemModelChanged: {
			if (currentItemModel === null) {
				return
			}

			ProviderCategoryFilterModel.setCategorySelection(currentItemModel.modelData.category)
		}

		Component {
			id: content

			Item {
				height: tabbedPane.availableHeight
				width: parent.width

				GridView {
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
					highlight: null
					activeFocusOnTab: true

					model: ProviderCategoryFilterModel

					boundsBehavior: Flickable.StopAtBounds
					ScrollBar.vertical: ScrollBar {
						policy: size === 1.0 ? ScrollBar.AlwaysOff : ScrollBar.AlwaysOn
					}

					delegate: Item {
						width: gridView.cellWidth
						height: gridView.cellHeight

						ProviderCard {
							anchors.fill: parent
							anchors.margins: Math.floor(gridView.spacing / 2)

							focus: gridView.currentIndex === index
							providerModelItem: model

							onShowDetailView: baseItem.showDetailView(pModelItem)
						}
					}

					Connections {
						target: ProviderCategoryFilterModel
						onFireCriteriaChanged: gridView.contentY = gridView.originY
					}
				}

				AdditionalResultsItem {
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

					onActiveFocusChanged: {
						if (activeFocus) {
							gridView.positionViewAtIndex(index, GridView.Center)
						}
					}

					onClicked: {
						ProviderCategoryFilterModel.setCategorySelection("all")
						tabbedPane.currentIndex = 0
					}
				}
			}
		}
	}

	FocusPoint {}
}
