import QtQuick 2.6

import "../.."
import "../../.."
import "../../../global"


Item {
	id: baseItem

	property TitleBarMenuAction leftTitleBarAction: TitleBarMenuAction {}
	property TitleBarAction headerTitleBarAction: TitleBarAction { text: qsTr("Provider"); font.bold: true }
	property Item rightTitleBarAction: TitleBarSearchAction {
		availableWidth: baseItem.width - leftTitleBarAction.contentWidth
		onSearchTextChanged: providerModel.categoryFilter.updateSearchString(searchText)
	}

	property color titleBarColor: Constants.blue

	readonly property real titleBarOpacity: 1
	readonly property int headerHeight: Utils.dp(54)
	readonly property int separatorHeight: Utils.dp(2)

	visible: false

	ProviderDetailView {
		id: providerDetailView
		visible: false
	}

	function pushProviderDetails(providerDetails) {
		historyModel.nameFilter.setProviderAddress(providerDetails['address'])
		push(providerDetailView, providerDetails)
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

				CategoryCheckbox {
					id: checkBoxCitizen

					category: "citizen"
					imageSource: Category.imageSource("citizen")
					text: qsTr("Citizen services")
				}

				CategoryCheckbox {
					id: checkBoxInsurance

					category: "insurance"
					imageSource: Category.imageSource("insurance")
					text: qsTr("Insurances")
				}

				CategoryCheckbox {
					id: checkBoxFinance

					category: "finance"
					imageSource: Category.imageSource("finance")
					text: qsTr("Financials")
				}

				CategoryCheckbox {
					id: checkBoxOther

					category: "other"
					imageSource: Category.imageSource("other")
					text: qsTr("Other services")
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

				anchors.centerIn: mainPane
				text: qsTr("No results found")

				wrapMode: Text.WordWrap
				font.pixelSize: Utils.sp(18)
				visible: !flickable.visible
			}

			Flickable {
				id: flickable

				readonly property int paddingWidth: Utils.dp(10)
				readonly property int spacingWidth: Utils.dp(20)

				clip: true
				flickableDirection: Flickable.VerticalFlick
				visible: grid.hasResults

				contentHeight: grid.height
				contentWidth: parent.width
				height: mainPane.height
				width: mainPane.width

				anchors.centerIn: parent

				Grid {
					id: grid
					columns: 4
					padding: flickable.paddingWidth
					spacing: flickable.spacingWidth
					width: parent.width

					property int cardHeight: (flickable.height - 2 * grid.padding - grid.spacing) / 2
					property int cardWidth: (flickable.width - 2 * grid.padding - (grid.columns - 1) *
												grid.spacing) / grid.columns
					property bool hasResults: gridRepeater.count > 0 || additionalResults.visible

					Repeater {
						id: gridRepeater

						model: providerModel.categoryFilter

						ProviderCard {
							pushFunction: baseItem.pushProviderDetails

							height: grid.cardHeight
							width: grid.cardWidth

							providerCategory: category
							headerImage: image
							headerIcon: icon
							providerName: longName !== "" ? longName : shortName
							providerShortDescription: shortDescription
							providerLongDescription: longDescription
							providerAddress: address
							providerHomepageBase: homepagebase
							providerEmail: email
							providerPhone: phone
							providerPostalAddress: postalAddress
						}
					}

					AdditionalResultsCard {
						id: additionalResults

						height: grid.cardHeight
						width: grid.cardWidth
					}
				}
			}
		}
	}
}
