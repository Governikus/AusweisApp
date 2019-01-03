import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0

Item {
	id: historyControls
	property var deleteHistoryConfirmationPopup: HistoryViewConfirmationPopup { }
	anchors.verticalCenter: parent ? parent.verticalCenter : undefined

	GSwitch {
		id: enableHistorySwitch
		anchors.verticalCenter: parent.verticalCenter
		anchors.rightMargin: Utils.dp(20)
		anchors.right: deleteEntriesButton.left

		color: Constants.green
		initialState: settingsModel.historyEnabled
		onSwitched: {
			settingsModel.historyEnabled = enableHistorySwitch.isOn
			qmlExtension.showFeedback((settingsModel.historyEnabled ? qsTr("History enabled") : qsTr("History disabled")) + settingsModel.translationTrigger)
		}
	}

	MouseArea {
		id: deleteEntriesButton
		width: deleteEntriesButtonImage.width
		height: deleteEntriesButtonImage.height
		anchors.verticalCenter: parent.verticalCenter
		anchors.right: parent.right
		anchors.rightMargin: Utils.dp(10)

		onClicked: {
			deleteHistoryConfirmationPopup.setValues("ALL_HISTORY", qsTr("Please confirm that you want to delete your complete history."))
			deleteHistoryConfirmationPopup.open()
		}

		Image {
			id: deleteEntriesButtonImage
			height: Constants.titlebar_font_size * 1.5
			fillMode: Image.PreserveAspectFit
			source: "qrc:///images/trash_icon_white.svg"
		}
	}
 }
