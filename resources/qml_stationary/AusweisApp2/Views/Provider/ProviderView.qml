import AusweisApp2.Global 1.0
import AusweisApp2.Views.ProviderDetails 1.0

import QtQml 2.2
import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3


Rectangle {
	id: baseItem
	property color titleBarColor: Constants.blue
	readonly property real titleBarOpacity: 1
	readonly property int headerHeight: Utils.dp(54)
	readonly property int separatorHeight: Utils.dp(2)
	color: "white"

	Column {
		id: header
		width: parent.width
		spacing: Constants.pane_spacing

		Text {
			width: parent.width
			text: qsTr("<html><head><body><p>This section lists offers of service providers that support online identification. " +
						"Click on an entry to go to the provider&apos;s web site. This section will be continuously updated with further applications for " +
						"the online identification function.</p></body></html>")
			wrapMode: Text.Wrap
		}

		Item {
			height: searchField.height
			width: parent.width

			Text {
				id: searchLabel
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Search:")
				textFormat: Text.StyledText
			}
			TextField {
				id: searchField
				anchors.left: searchLabel.right
				anchors.leftMargin: Constants.pane_spacing
				anchors.right: parent.right
				text: ""
				onTextChanged: providerModel.searchString = text
			}
		}

		Row {
			id: checkBoxesItem
			height: Utils.dp(40)
			anchors.horizontalCenter: parent.horizontalCenter
			padding: space
			readonly property real space: (parent.width - checkBoxCitizen.width - checkBoxInsurance.width - checkBoxFinance.width - checkBoxOther.width) / 5
			spacing: space

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

	Item {
		anchors.top: header.bottom
		anchors.topMargin: header.spacing
		anchors.bottom: parent.bottom
		width: parent.width

		Text {
			anchors.centerIn: parent
			text: qsTr("No match found")
			wrapMode: Text.WordWrap
			font.pixelSize: Constants.normal_font_size
			visible: !scrollView.visible
		}
		ScrollView {
			id: scrollView
			anchors.fill: parent
			anchors.topMargin: 1
			anchors.bottomMargin: 1
			clip: true
			horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
			verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn
			visible: tableRepeater.count > 0 || additionalResults.totalHits > 0

			Rectangle {
				width: baseItem.width
				height: table.height
				color: PlatformConstants.grey_border

				Column {
					id: table
					width: parent.width
					spacing: 1

					Repeater {
						id: tableRepeater
						model: providerModel

						ProviderCard {
							height: Utils.dp(120)
							width: parent.width
							providerModelItem: model
							onShowProviderDetails: {
								historyModel.nameFilter.setProviderAddress(model.providerAddress)
								providerDetailsDialog.providerModelItem = model
								providerDetailsDialog.visible = true
							}
						}
					}
					AdditionalResultsItem {
						id: additionalResults
						width: parent.width
						height: Utils.dp(120)
					}
				}
			}
		}

		Rectangle {
			anchors.top: parent.top
			width: parent.width
			height: 1
			color: PlatformConstants.grey_border
		}
		Rectangle {
			anchors.bottom: parent.bottom
			width: parent.width
			height: 1
			color: PlatformConstants.grey_border
		}
	}


	ProviderDetailsDialog {
		id: providerDetailsDialog
		visible: false
	}
}
