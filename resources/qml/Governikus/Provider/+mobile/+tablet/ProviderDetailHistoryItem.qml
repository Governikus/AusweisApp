import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: baseItem

	readonly property color backgroundColor: Constants.blue
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
			text: (!new Date(dateTime) ? "" :
				  Utils.isToday(dateTime) ? qsTr("today") :
				  Utils.isYesterday(dateTime) ? qsTr("yesterday") :
				  Utils.isThisWeek(dateTime) ? dateTime.toLocaleString(Qt.locale(), qsTr("dddd")) :
				  dateTime.toLocaleString(Qt.locale(), qsTr("dd.MM.yyyy"))
				  ) + settingsModel.translationTrigger
		}

		LabeledText {
			label: !!purposeText ? purposeText : ""
			text: (!!providerName ? providerName : qsTr("Touch for more details")) + settingsModel.translationTrigger
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

			border.color: ProviderStyle.providerListDetailsLinkBorder
			border.width: 1
			radius: width

			color: ProviderStyle.providerListDetailsLinkBackground
			Text {
				anchors.centerIn: parent

				text: "i"
				font.bold: ProviderStyle.providerListDetailsLinkBold
				color: ProviderStyle.providerListDetailsLinkColor
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
