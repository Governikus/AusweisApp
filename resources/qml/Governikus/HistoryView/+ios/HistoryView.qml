import QtQml.Models 2.2
import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Governikus.Global 1.0
import Governikus.TitleBar 1.0

SectionPage {
	leftTitleBarAction: TitleBarAction {
		id: leftAction

		state: "edit"
		states: [
			State { name: "edit"; when: !historyListView.selectable },
			State { name: "cancel"; when: historyListView.selectable }
		]
		onClicked: {
			if (state === "edit") {
				state = "cancel"
				historyListView.selectable = true
			}
			else {
				state = "edit"
				historyListView.selectable = false
				historyListView.cancelDeletion()
			}
		}
	}

	headerTitleBarAction: Text { text: qsTr("History") + settingsModel.translationTrigger; font.bold: true }

	rightTitleBarAction: TitleBarAction {
		id: rightAction

		states: [
			State {
				name: "none"
				when: leftAction.state == "edit"
				PropertyChanges { target: rightAction; text: "" }
			},
			State {
				name: "delete"
				when: historyListView.selectable && historyListView.selectedIndices.length !== 0
				PropertyChanges { target: rightAction; text: qsTr("Delete") + settingsModel.translationTrigger }
			},
			State {
				name: "deleteAll"
				when: historyListView.selectable && historyListView.selectedIndices.length === 0
				PropertyChanges { target: rightAction; text: qsTr("Delete all") + settingsModel.translationTrigger }
			}
		]
		onClicked: {
			historyListView.performDeletion()
			historyListView.selectable = false
		}
	}

	HistoryViewBackground {
		visible: historyListView.count !== 0
	}

	Text {
		anchors.centerIn: parent
		text: qsTr("Currently there are no history entries.") + settingsModel.translationTrigger
		wrapMode: Text.WordWrap
		font.pixelSize: Constants.normal_font_size
		visible: historyListView.count === 0
	}

	HistoryListView {
		id: historyListView
		anchors.fill: parent

		listViewModel: historyModel
		delegate: HistoryListViewDelegate {
			id: historyDelegate
			showDetail: false
		}

		onSelectedIndicesChanged: {
			if (!historyListView.selectable) {
				leftAction.state = historyListView.selectedIndices.length !== 0 ? "cancel" : "edit"
			}
		}
	}

	HistoryViewPage {
		id: providerHistoryView
		visible: false
	}

	HistoryViewDetails {
		id: detailsHistoryView
		visible: false
	}
}
