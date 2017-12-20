import AusweisApp2.Global 1.0

import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Item {
	id: baseItem
	property var historyModelItem
	readonly property string providerName: !!historyModelItem.providerLongName ? historyModelItem.providerLongName : historyModelItem.providerShortName


	Text {
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.verticalCenter
		anchors.margins: Utils.dp(15)
		anchors.bottomMargin: 0

		verticalAlignment: Text.AlignVCenter
		font.capitalization: Font.AllUppercase
		elide: Text.ElideRight
		text: {
			if (!historyModelItem) {
				return "";
			}
			else if (Utils.isToday(historyModelItem.dateTime)) {
				return qsTr("today") + settingsModel.translationTrigger
			}
			else if (Utils.isYesterday(historyModelItem.dateTime)) {
				return qsTr("yesterday") + settingsModel.translationTrigger
			}
			else if (Utils.isThisWeek(historyModelItem.dateTime)) {
				return historyModelItem.dateTime.toLocaleString(Qt.locale(), qsTr("dddd")) + settingsModel.translationTrigger
			}
			return historyModelItem.dateTime.toLocaleString(Qt.locale(), qsTr("dd.MM.yyyy")) + settingsModel.translationTrigger
		}
	}
	Text {
		anchors.top: parent.verticalCenter
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.margins: Utils.dp(15)
		anchors.topMargin: 0

		verticalAlignment: Text.AlignVCenter
		font.pixelSize: Constants.small_font_size
		elide: Text.ElideRight
		color: Constants.blue_dark
		text: !!providerName ? providerName : historyModelItem.subject
	}
}
