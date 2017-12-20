import QtQuick 2.6

import Governikus.Global 1.0
import Governikus.Provider 1.0

Item {
	id: baseItem

	property string providerName: ""
	property string providerPostalAddress: ""
	property string purposeText: ""
	property string requestedDataText: ""
	property string termsOfUsageText: ""
	property string internalState: "off"

	Rectangle {
		anchors.fill: baseItem

		color: "black"
		opacity: 0.4
	}

	Flickable {
		anchors.fill: baseItem
		anchors.margins: Constants.component_spacing
		contentHeight: infoRow.height

		onContentYChanged: {
			if (contentY < 0) { contentY = 0 /* prevent flicking over the top */}
		}

		Row {
			id: infoRow
			height: Math.max(leftColumn.height, rightColumn.height) + 2 * Constants.pane_padding
			spacing: Constants.component_spacing

			Item {
				height: 1
				width: baseItem.width / 3

				Pane {
					id: leftPane
					height: infoRow.height
				}

				Column {
					id: leftColumn
					anchors.margins: Constants.pane_padding
					anchors.left: parent.left
					anchors.top: parent.top
					anchors.right: parent.right
					spacing: Constants.pane_spacing

					ProviderInfoSection {
						imageSource: "qrc:///images/provider/information.svg"
						title: qsTr("Service provider") + settingsModel.translationTrigger
						name: baseItem.providerName
					}

					ProviderInfoSection {
						imageSource: "qrc:///images/provider/purpose.svg"
						title: qsTr("Purpose for reading out requested data") + settingsModel.translationTrigger
						name: baseItem.purposeText
					}

					Text {
						id: readDataTitle
						width: parent.width
						font.pixelSize: Constants.header_font_size
						color: Constants.blue
						text: qsTr("Read data") + settingsModel.translationTrigger
					}

					Column {
						id: infoTable

						width: parent.width
						spacing: 1

						Repeater {
							model: baseItem.requestedDataText.split(",")

							Item {
								id: textItem

								height: Utils.dp(32)
								width: infoTable.width

								Rectangle {
									anchors.fill: textItem
									color: "white"
								}

								Text {
									text: modelData.trim()

									anchors.verticalCenter: parent.verticalCenter
									font.pixelSize: Constants.normal_font_size
								}
							}
						}
					}
				}
			}

			Item {
				height: 1
				width: baseItem.width / 3 * 2 - 3 * Constants.component_spacing

				Pane {
					id: rightPane
					height: infoRow.height
				}

				Column {
					id: rightColumn
					anchors.margins: Constants.pane_padding
					anchors.left: parent.left
					anchors.top: parent.top
					anchors.right: parent.right
					spacing: Constants.pane_spacing

					Text {
						id: termsOfUsageTitle
						text: qsTr("Terms of usage") + settingsModel.translationTrigger
						font.pixelSize: Constants.header_font_size
						color: Constants.blue
					}

					Text {
						id: termsOfUsageTextItem

						text: baseItem.termsOfUsageText
						width: parent.width
						elide: Text.ElideRight
						wrapMode: Text.Wrap
						font.pixelSize: Constants.normal_font_size
					}
				}
			}
		}
	}
}
