import QtQuick 2.6

import Governikus.Global 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.Provider 1.0

SectionPage {
	id: baseItem

	leftTitleBarAction: TitleBarAction {}
	headerTitleBarAction: TitleBarAction { text: qsTr("Provider") + settingsModel.translationTrigger; font.bold: true }
	rightTitleBarAction: SearchBar {
		availableWidth: baseItem.width
		onSearchTextChanged: providerModel.searchString = searchText
	}

	titleBarColor: Constants.blue

	visible: false
	property bool wasVisible: false
	onVisibleChanged: wasVisible = true

	readonly property int headerHeight: Utils.dp(54)
	readonly property int separatorHeight: Utils.dp(2)

	ProviderDetailView_tablet {
		id: providerDetailView
		visible: false
	}

	function pushProviderDetails(model) {
		historyModel.nameFilter.setProviderAddress(model.providerAddress)
		firePush(providerDetailView, {providerModelItem: model})
	}

	Column {
		id: content

		width: parent.width

		Rectangle {

			height: baseItem.headerHeight
			width: parent.width

			color: "white"

			Row {
				id: checkBoxesItem

				height: parent.height
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter

				padding: Utils.dp(30)
				spacing: Utils.dp(30)

				transformOrigin: Item.Center
				scale: Math.min(parent.width / width, 1)

				CategoryCheckbox_tablet {
					id: checkBoxCitizen

					category: "citizen"
					imageSource: Category.imageSource("citizen")
					text: qsTr("Citizen services") + settingsModel.translationTrigger
				}

				CategoryCheckbox_tablet {
					id: checkBoxFinance

					category: "finance"
					imageSource: Category.imageSource("finance")
					text: qsTr("Financials") + settingsModel.translationTrigger
				}

				CategoryCheckbox_tablet {
					id: checkBoxInsurance

					category: "insurance"
					imageSource: Category.imageSource("insurance")
					text: qsTr("Insurances") + settingsModel.translationTrigger
				}

				CategoryCheckbox_tablet {
					id: checkBoxOther

					category: "other"
					imageSource: Category.imageSource("other")
					text: qsTr("Other services") + settingsModel.translationTrigger
				}
			}
		}

		Rectangle {
			height: baseItem.separatorHeight
			width: parent.width

			color: PlatformConstants.grey_border
		}

		Rectangle {
			id: mainPane

			height: baseItem.height - (baseItem.headerHeight + baseItem.separatorHeight)
			width: parent.width
			anchors.horizontalCenter: parent.horizontalCenter
			color: Constants.background_color

			Text {
				id: noResultsText
				color: Constants.secondary_text

				anchors.centerIn: mainPane
				text: qsTr("No match found") + settingsModel.translationTrigger

				wrapMode: Text.WordWrap
				font.pixelSize: Constants.normal_font_size
				visible: !flickable.visible
			}

			Flickable {
				id: flickable
				anchors.fill: mainPane
				clip: true
				flickableDirection: Flickable.VerticalFlick
				visible: grid.hasResults

				contentHeight: grid.height
				contentWidth: parent.width

				onContentYChanged: {
					if (contentY < 0) { contentY = 0 /* prevent flicking over the top */}
				}

				Grid {
					id: grid
					columns: Math.floor((parent.width - Constants.component_spacing) / (Utils.dp(196) + Constants.component_spacing))
					padding: Constants.component_spacing
					spacing: Constants.component_spacing
					width: parent.width

					property int cardHeight: (flickable.height - Constants.component_spacing) / 2
					property int cardWidth: (flickable.width - (grid.columns + 1) * Constants.component_spacing) / grid.columns
					property bool hasResults: gridRepeater.count > 0 || additionalResults.totalHits > 0

					Repeater {
						id: gridRepeater

						model: providerModel

						ProviderCard_tablet {
							width: grid.cardWidth
							headerHeight: width / 1.80
							textHeight: Utils.dp(64)
							footerHeight: Utils.dp(30)
							pushFunction: baseItem.pushProviderDetails
							providerModelItem: baseItem.wasVisible ? model : undefined
						}
					}

					AdditionalResultsItem_tablet {
						id: additionalResults
						width: grid.cardWidth
						headerHeight: width / 1.80
						textHeight: Utils.dp(64)
						footerHeight: Utils.dp(30)
					}
				}
			}
		}
	}
}
