import QtQuick 2.6

import ".."
import "../global"


Rectangle {
	id: baseItem

	property string providerName: ""
	property string providerPostalAddress: ""
	property string purposeText: ""
	property string requestedDataText: ""
	property string termsOfUsageText: ""
	property string internalState: "off"

	color: "transparent"

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
			height: childrenRect.height
			spacing: Constants.component_spacing

			property int maxContentHeight: Math.max(leftPane.contentHeight, rightPane.contentHeight)

			Column {
				width: baseItem.width / 3

				Pane {
					id: leftPane
					height: childrenRect.height + verticalSpace

					readonly property int contentHeight: childrenRect.height
					property int verticalSpace: infoRow.maxContentHeight - contentHeight

					ProviderInfoSection {
						imageSource: "qrc:///images/provider/information.svg"
						title: qsTr("Service provider")
						name: baseItem.providerName
					}

					ProviderInfoSection {
						imageSource: "qrc:///images/provider/purpose.svg"
						title: qsTr("Purpose for reading out requested data")
						name: baseItem.purposeText
					}

					Text {
						id: readDataTitle
						width: parent.width
						font.pixelSize: Constants.header_font_size
						color: PlatformConstants.blue_primary
						text: qsTr("Read data")
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

			Column {
				width: baseItem.width / 3 * 2 - 3 * Constants.component_spacing

				Pane {
					id: rightPane
					height: childrenRect.height + verticalSpace

					readonly property int contentHeight: childrenRect.height
					property int verticalSpace: infoRow.maxContentHeight - contentHeight

					Text {
						id: termsOfUsageTitle
						text: qsTr("Terms of usage")
						font.pixelSize: Constants.header_font_size
						color: PlatformConstants.blue_primary
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
