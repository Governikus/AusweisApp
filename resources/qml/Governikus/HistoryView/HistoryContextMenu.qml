import QtQuick 2.7
import QtQuick.Controls 2.2

import Governikus.Global 1.0

MouseArea {
	property var historyReminderPopup: HistoryViewConfirmationPopup { }
	readonly property alias contentWidth: contextMenuButton.width

	id: contextMenuButton
	height: Constants.titlebar_height
	width: height
	anchors.right: parent ? parent.right : undefined
	anchors.top: parent ? parent.top : undefined
	anchors.verticalCenter: parent ? parent.verticalCenter : undefined

	Image {
		id: contextMenuIcon
		height: Constants.titlebar_height - 2 * Constants.titlebar_padding
		width: height
		anchors.topMargin: Constants.titlebar_padding
		anchors.right: parent.right
		anchors.bottomMargin: Constants.titlebar_padding
		source: "qrc:///images/android/navigation/versionsinformation.svg"
		anchors.verticalCenter: parent.verticalCenter
	}

	onClicked: contextMenu.visible ? contextMenu.close() : contextMenu.open()

	Popup {
		id: contextMenu
		x: -(width - contextMenuButton.width)
		y: Constants.titlebar_height + Constants.titlebar_padding
		closePolicy: Popup.CloseOnPressOutsideParent | Popup.CloseOnEscape
		height: contentColumn.height + Constants.titlebar_padding

		Column {
			id: contentColumn
			width: childrenRect.width
			spacing: Constants.titlebar_padding

			ContextMenuEntry {
				text: (settingsModel.historyEnabled ? qsTr("Disable history") : qsTr("Enable history")) + settingsModel.translationTrigger
				onClicked: {
					settingsModel.historyEnabled = !settingsModel.historyEnabled
					qmlExtension.showFeedback((settingsModel.historyEnabled ? qsTr("History enabled") : qsTr("History disabled")) + settingsModel.translationTrigger)
					contextMenu.close()
				}
			}

			Rectangle {
				width: parent.width
				color: Constants.grey
				height: Utils.dp(1)
			}

			ContextMenuEntry {
				text: qsTr("Delete all") + settingsModel.translationTrigger
				onClicked: {
					historyReminderPopup.setValues("ALL_HISTORY", qsTr("Please confirm that you want to delete your complete history."))
					historyReminderPopup.open()
					contextMenu.close()
				}
			}

			ContextMenuEntry {
				text: qsTr("Delete last 4 weeks") + settingsModel.translationTrigger
				onClicked: {
					historyReminderPopup.setValues("LAST_FOUR_WEEKS", qsTr("Please confirm that you want to delete your history from the last four weeks."))
					historyReminderPopup.open()
					contextMenu.close()
				}
			}

			ContextMenuEntry {
				text: qsTr("Delete last week") + settingsModel.translationTrigger
				onClicked: {
					historyReminderPopup.setValues("PAST_WEEK", qsTr("Please confirm that you want to delete your history from the last week."))
					historyReminderPopup.open()
					contextMenu.close()
				}
			}

			ContextMenuEntry {
				text: qsTr("Delete last day") + settingsModel.translationTrigger
				onClicked: {
					historyReminderPopup.setValues("PAST_DAY", qsTr("Please confirm that you want to delete your history from the last day."))
					historyReminderPopup.open()
					contextMenu.close()
				}
			}

			ContextMenuEntry {
				text: qsTr("Delete last hour") + settingsModel.translationTrigger
				onClicked: {
					historyReminderPopup.setValues("PAST_HOUR", qsTr("Please confirm that you want to delete your history from the last hour."))
					historyReminderPopup.open()
					contextMenu.close()
				}
			}
		}
	}
}
