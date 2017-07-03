import QtQuick 2.6

import "../global"


Item {
	id: baseItem

	readonly property color backgroundColor: PlatformConstants.blue
	readonly property int iconWidth: Utils.dp(18)

	property string providerName: ""
	property string providerPostalAddress: ""
	property string timeText: ""
	property string infoText: ""
	property string purposeText: ""
	property string requestedDataText: ""
	property string termsOfUsageText: ""
	property int itemHeight: 0
	property int itemMargin: 0
	property int lineHeight: itemHeight / 3
	property var openInfoFunction: function () {}

	height: itemHeight + 2 * itemMargin

	ProviderStyle {
		id: providerStyle

		visible: false
	}

	Rectangle {
		anchors.fill: baseItem
		color: "white"
	}

	Column {
		id: textColumn

		height: baseItem.itemHeight
		width: baseItem.width - baseItem.iconWidth

		anchors.left: baseItem.left
		anchors.top: baseItem.top
		anchors.topMargin: baseItem.itemMargin

		Text {
			text: timeText
			height: baseItem.lineHeight
			color: PlatformConstants.blue_primary
		}

		Text {
			text: infoText
			height: baseItem.lineHeight
		}

		Text {
			text: purposeText
			height: baseItem.lineHeight
			color: PlatformConstants.blue_primary
			font.bold: true
			font.pixelSize: Constants.normal_font_size
			elide: Text.ElideRight
			maximumLineCount: 1
		}
	}

	Item {
		id: detailsLink

		anchors.right: parent.right
		anchors.verticalCenter:	parent.verticalCenter

		height: width
		width: iconWidth

		Rectangle {
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.verticalCenter: parent.verticalCenter

			height: width

			border.color: providerStyle.providerListDetailsLinkBorder
			border.width: 1
			radius: width

			color: providerStyle.providerListDetailsLinkBackground
			Text {
				anchors.centerIn: parent

				text: "i"
				font.bold: providerStyle.providerListDetailsLinkBold
				color: providerStyle.providerListDetailsLinkColor
			}
		}
	}

	MouseArea {
		anchors.fill: baseItem
		onClicked: baseItem.openInfoFunction({
			providerName: baseItem.providerName,
			providerPostalAddress: baseItem.providerPostalAddress,
			purposeText: baseItem.purposeText,
			requestedDataText: baseItem.requestedDataText,
			termsOfUsageText: baseItem.termsOfUsageText
		})
	}
}
