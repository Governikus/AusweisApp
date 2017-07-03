import QtQml.Models 2.2
import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import "../"
import "../global"

SectionPage {
	leftTitleBarAction: TitleBarMenuAction {
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
				PropertyChanges { target: rightAction; text: qsTr("Delete") }
			},
			State {
				name: "deleteAll"
				when: historyListView.selectable && historyListView.selectedIndices.length === 0
				PropertyChanges { target: rightAction; text: qsTr("Delete all") }
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
		text: qsTr("Currently there are no history entries.")
		wrapMode: Text.WordWrap
		font.pixelSize: Utils.sp(18)
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
