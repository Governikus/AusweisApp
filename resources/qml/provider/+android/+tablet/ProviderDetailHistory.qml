import QtQuick 2.6

import "../global"


Rectangle {
	id: baseItem

	color: Constants.background_color

	property string providerName: ""
	property var openHistoryInfoFunc: function() {
	}

	readonly property int frameThickness: Utils.dp(10)
	readonly property int historySpacing: Utils.dp(1)
	readonly property int historyItemHeight: Utils.dp(66)
	readonly property int historyItemMargin: Utils.dp(8)

	Rectangle {
		id: frame

		radius: Utils.dp(5)
		height: baseItem.height - 2 * baseItem.frameThickness
		width: baseItem.width - 2 * baseItem.frameThickness
		anchors.top: parent.top
		anchors.topMargin: baseItem.frameThickness
		anchors.left: parent.left
		anchors.leftMargin: baseItem.frameThickness

		color: "white"

		Column {
			id: mainColumn

			spacing: baseItem.frameThickness
			height: frame.height - 2 * frame.radius
			width: frame.width - 2 * frame.radius
			anchors.top: parent.top
			anchors.topMargin: parent.radius
			anchors.left: parent.left
			anchors.leftMargin: parent.radius

			Text {
				id: headerText

				font.pixelSize: Constants.header_font_size
				color: PlatformConstants.blue_primary
				text: qsTr("History")
			}

			Flickable {
				id: history

				height: mainColumn.height - mainColumn.spacing - headerText.height
				width: mainColumn.width

				contentHeight: infoTable.height
				contentWidth: width
				clip: true
				flickableDirection: Flickable.VerticalFlick
				boundsBehavior: Flickable.StopAtBounds

				Rectangle {
					anchors.fill: infoTable

					color: "black"
				}

				Column {
					id: infoTable

					width: history.width
					spacing: baseItem.historySpacing

					Repeater {
						model: historyModel.nameFilter

						ProviderDetailHistoryItem {
							itemHeight: baseItem.historyItemHeight
							itemMargin: baseItem.historyItemMargin

							width: parent.width

							providerName: subject
							providerPostalAddress: provider_postaladdress
							timeText: dateTime
							infoText: qsTr("Purpose for reading out requested data")
							purposeText: purpose
							requestedDataText: requestedData
							termsOfUsageText: termsOfUsage

							openInfoFunction: baseItem.openHistoryInfoFunc
						}
					}
				}
			}
		}
	}
}
