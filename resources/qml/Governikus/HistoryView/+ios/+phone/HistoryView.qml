import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0

SectionPage {
	id: baseItem

	leftTitleBarAction: TitleBarAction {
		id: leftAction

		state: "edit"
		states: [
			State { name: "edit"; when: !historyListView.editMode },
			State { name: "cancel"; when: historyListView.editMode }
		]
		onClicked: {
			if (state === "edit") {
				state = "cancel"
				historyListView.editMode = true
			}
			else {
				state = "edit"
				historyListView.editMode = false
			}
		}
	}

	headerTitleBarAction: Text { text: qsTr("History") + settingsModel.translationTrigger; font.bold: true }

	rightTitleBarAction: TitleBarAction {
		id: rightAction

		states: [
			State {
				name: "none"
				when: leftAction.state === "edit"
				PropertyChanges { target: rightAction; text: "" }
			},
			State {
				name: "deleteAll"
				when: historyListView.editMode
				PropertyChanges { target: rightAction; text: qsTr("Delete all") + settingsModel.translationTrigger }
			}
		]
		onClicked: {
			if (historyListView.editMode){
				deleteHistoryConfirmationPopup.setValues("ALL_HISTORY", qsTr("Please confirm that you want to delete your complete history."))
				deleteHistoryConfirmationPopup.open()
			}
			historyListView.editMode = false
		}
	}

	HistoryViewConfirmationPopup {
		id: deleteHistoryConfirmationPopup
		baseItem: baseItem
	}

	Text {
		color: Constants.secondary_text
		anchors.centerIn: parent
		text: qsTr("Currently there are no history entries.") + settingsModel.translationTrigger
		wrapMode: Text.WordWrap
		font.pixelSize: Constants.normal_font_size
		visible: historyListView.count === 0
	}

	ListView {
		id: historyListView
		anchors.fill: parent
		focus: true
		spacing: Utils.dp(5)

		property bool editMode: false

		model: historyModel
		delegate: HistoryListViewDelegate {
			historyModelItem: model

			onClicked: {
				detailsHistoryView.historyModelItem = historyModelItem
				firePush(detailsHistoryView)
			}

			Connections {
				target: historyListView
				onEditModeChanged: {
					reactToEditModeChanged(historyListView.editMode)
				}
			}
		}
	}

	HistoryViewDetails {
		id: detailsHistoryView
		visible: false
	}
}
