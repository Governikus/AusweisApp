/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.HistoryModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0


SectionPage {
	id: baseItem

	navigationAction: NavigationAction {}
	title: qsTr("Provider") + SettingsModel.translationTrigger
	rightTitleBarAction: SearchBar {
		availableWidth: baseItem.width
		onSearchTextChanged: ProviderCategoryFilterModel.searchString = searchText
	}

	titleBarColor: Constants.blue

	visible: false
	property bool wasVisible: false
	onVisibleChanged: wasVisible = true

	readonly property int headerHeight: 54
	readonly property int separatorHeight: 2

	ProviderDetailView {
		id: providerDetailView
		visible: false
	}

	function pushProviderDetails(pModel) {
		HistoryModel.nameFilter.setProviderAddress(pModel.providerAddress)
		providerDetailView.providerModelItem = pModel
		firePush(providerDetailView)
	}

	Column {
		id: content

		width: parent.width

		Rectangle {

			height: baseItem.headerHeight
			width: parent.width

			color: Constants.white

			Row {
				id: checkBoxesItem

				height: parent.height
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter

				padding: 30
				spacing: 30

				transformOrigin: Item.Center
				scale: Math.min(parent.width / width, 1)

				CategoryCheckbox {
					id: checkBoxCitizen

					category: "citizen"
					imageSource: Category.imageSource("citizen")
					//: LABEL ANDROID_TABLET
					text: qsTr("Citizen services") + SettingsModel.translationTrigger
				}

				CategoryCheckbox {
					id: checkBoxFinance

					category: "finance"
					imageSource: Category.imageSource("finance")
					//: LABEL ANDROID_TABLET
					text: qsTr("Financials") + SettingsModel.translationTrigger
				}

				CategoryCheckbox {
					id: checkBoxInsurance

					category: "insurance"
					imageSource: Category.imageSource("insurance")
					//: LABEL ANDROID_TABLET
					text: qsTr("Insurances") + SettingsModel.translationTrigger
				}

				CategoryCheckbox {
					id: checkBoxOther

					category: "other"
					imageSource: Category.imageSource("other")
					//: LABEL ANDROID_TABLET
					text: qsTr("Other services") + SettingsModel.translationTrigger
				}
			}
		}

		Rectangle {
			height: baseItem.separatorHeight
			width: parent.width

			color: Style.color.border
		}

		Rectangle {
			id: mainPane

			height: baseItem.height - (baseItem.headerHeight + baseItem.separatorHeight)
			width: parent.width
			anchors.horizontalCenter: parent.horizontalCenter
			color: Style.color.background

			GText {
				id: noResultsText

				anchors.centerIn: mainPane
				//: LABEL ANDROID_TABLET The text entered into the provider search field results in no matches
				text: qsTr("No results matching your search query found") + SettingsModel.translationTrigger
				textStyle: Style.text.normal

				visible: !flickable.visible
			}

			GFlickable {
				id: flickable
				anchors.fill: mainPane
				clip: true
				visible: grid.hasResults

				contentHeight: grid.height
				contentWidth: parent.width

				onContentYChanged: {
					if (contentY < 0) { contentY = 0 /* prevent flicking over the top */}
				}

				Grid {
					id: grid
					columns: Math.floor((parent.width - Constants.component_spacing) / (196 + Constants.component_spacing))
					padding: Constants.component_spacing
					spacing: Constants.component_spacing
					width: parent.width

					property int cardHeight: (flickable.height - Constants.component_spacing) / 2
					property int cardWidth: (flickable.width - (grid.columns + 1) * Constants.component_spacing) / grid.columns
					property bool hasResults: gridRepeater.count > 0 || additionalResults.totalHits > 0

					Repeater {
						id: gridRepeater

						model: ProviderCategoryFilterModel

						ProviderCard {
							width: grid.cardWidth
							headerHeight: width / 1.80
							textHeight: 64
							footerHeight: 30
							pushFunction: baseItem.pushProviderDetails
							providerModelItem: baseItem.wasVisible ? model : undefined
						}
					}

					AdditionalResultsItem {
						id: additionalResults
						width: grid.cardWidth
						headerHeight: width / 1.80
						textHeight: 64
						footerHeight: 30
					}
				}
			}
		}
	}
}
