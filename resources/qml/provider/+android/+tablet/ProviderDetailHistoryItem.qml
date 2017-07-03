import QtQuick 2.6

import "../global"


Item {
	id: baseItem

	readonly property color backgroundColor: PlatformConstants.blue
	readonly property int iconWidth: Utils.dp(18)

	property string providerName: ""
	property string providerPostalAddress: ""
	property var dateTime: ""
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
			height: baseItem.lineHeight
			verticalAlignment: Text.AlignVCenter
			font.pixelSize: Constants.label_font_size
			font.capitalization: Font.AllUppercase
			color: Constants.blue
			text: {
				if (!new Date(dateTime)) {
					return ""
				}
				else if (Utils.isToday(dateTime)) {
					return qsTr("today")
				}
				else if (Utils.isYesterday(dateTime)) {
					return qsTr("yesterday")
				}
				else if (Utils.isThisWeek(dateTime)) {
					return dateTime.toLocaleString(Qt.locale(), qsTr("dddd"))
				}
				return dateTime.toLocaleString(Qt.locale(), qsTr("MM/dd/yyyy"))
			}
		}

		LabeledText {
			label: infoText
			text: !!purposeText ? purposeText : qsTr("Touch for more details")
			width: parent.width
			height: baseItem.lineHeight
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
