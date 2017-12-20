import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Governikus.Global 1.0

Item {
	id: baseItem

	property alias listViewModel: listView.model
	property alias interactive: listView.interactive
	readonly property int count: listView.count
	property bool deletable: true
	property bool selectable: false
	property var selectedIndices: []
	property Component delegate

	readonly property int contentHeight: listView.contentHeight

	function requestDeletion(index) {
		selectedIndices.push(index)
		selectedIndices = selectedIndices //to have the change signal emitted
	}


	function cancelDeletion(cancelIndex) {
		if (arguments.length === 1) {
			var reducedSelection = [];
			selectedIndices.map(function (index) {
				if (index !== cancelIndex) {
					reducedSelection.push(index)
				}
			})
			selectedIndices = reducedSelection
		}
		else {
			selectedIndices = []
		}
	}


	function performDeletion() {
		if (selectedIndices.length === 0) {
			listViewModel.removeRows(0, historyModel.rowCount())
		}
		else {
			 // sort in descending order, so that the indices don't change after partial deletion
			selectedIndices.sort(function(a,b){return b-a})
			selectedIndices.map(function (index) {
				listViewModel.removeRows(index, 1)
			})
			selectedIndices = [] //to have the change signal emitted
		}
	}


	ListView {
		id: listView
		anchors.fill: parent
		focus: true
		spacing: Utils.dp(5)

		delegate: Item {
			id: delegateItem
			width: parent.width
			height: delegateLoader.height

			Connections {
				target: baseItem
				onSelectedIndicesChanged: {
					if (baseItem.selectedIndices.indexOf(model.index) === -1) {
						flickable.state = "hideDeleteConfirmation"
						checkBox.checked = false
					}
				}
			}

			Rectangle {
				id: confirmDeletionRect
				height: parent.height
				width: parent.width / 4
				anchors.right: parent.right
				anchors.rightMargin: Utils.dp(10)
				anchors.top: parent.top
				anchors.topMargin: Utils.dp(8)
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Utils.dp(8)

				color: "red"

				Text {
					anchors.centerIn: parent
					text: qsTr("Delete") + settingsModel.translationTrigger
					color: "white"
				}
			}

			Flickable {
				id: flickable
				anchors.fill: parent
				contentWidth: parent.width + confirmDeletionRect.width
				contentHeight: parent.height
				flickableDirection: Flickable.HorizontalFlick
				interactive: baseItem.deletable && !baseItem.selectable && Constants.use_history_list_delete_area

				Item {
					id: delegateContent
					width: delegateItem.width
					height: delegateItem.height

					CheckBox {
						id: checkBox
						width: Utils.dp(40)
						height: parent.height

						style: IosCheckBoxStyle {}
						onClicked: checked ? requestDeletion(model.index) : cancelDeletion(model.index)
					}
					Loader {
						id: delegateLoader
						anchors.left: checkBox.right
						anchors.right: parent.right
						height: Constants.history_section_height
						sourceComponent: baseItem.delegate
						property var historyModelItem: model
					}

					states: [
						State {
							name: "notSelectable"
							when: !baseItem.selectable
							PropertyChanges { target: checkBox; opacity: 0 }
							AnchorChanges { target: checkBox; anchors.left: undefined}
							AnchorChanges { target: checkBox; anchors.right: delegateContent.left}
						},
						State {
							name: "selectable"
							when: baseItem.selectable
							PropertyChanges { target: checkBox; opacity: 1 }
							AnchorChanges { target: checkBox; anchors.left: delegateContent.left}
							AnchorChanges { target: checkBox; anchors.right: undefined}
						}
					]
					state: "notSelectable"

					transitions: [
						Transition {
							from: "notSelectable"
							to: "selectable"
							reversible: true
							animations:
							[
								AnchorAnimation { duration: 200 },
								PropertyAnimation { target: checkBox; properties: "opacity"; duration: 200 }
							]
						}
					]
				}

				onMovementStarted: {
					delegateItem.ListView.view.currentIndex = index
					state = "moving"
					requestDeletion(model.index)
				}
				onMovementEnded: {
					var ratio = contentX / confirmDeletionRect.width
					if (ratio > 0.4) {
						state = "showDeleteConfirmation"
					}
					else {
						state = "hideDeleteConfirmation"
						cancelDeletion(model.index)
					}
				}

				state: "hideDeleteConfirmation"
				states: [
					State {
						name: "moving"
					},
					State {
						name: "showDeleteConfirmation"
						PropertyChanges { target: flickable; contentX: confirmDeletionRect.width }
						PropertyChanges { target: cancelDeletionMouseArea; enabled: true }
						PropertyChanges { target: confirmDeletionMouseArea; enabled: true }
					},
					State {
						name: "hideDeleteConfirmation"
						PropertyChanges { target: flickable; contentX: 0 }
						PropertyChanges { target: cancelDeletionMouseArea; enabled: false }
						PropertyChanges { target: confirmDeletionMouseArea; enabled: false }
					}
				]
				Behavior on contentX {
					NumberAnimation { duration: 500; easing.type: Easing.OutQuart }
				}
			}
		}

		removeDisplaced: Transition {
			NumberAnimation { properties: "y"; duration: 500; easing.type: Easing.OutQuart }
		}
	}

	MouseArea {
		id: cancelDeletionMouseArea
		enabled: false
		anchors.fill: parent
		onClicked: cancelDeletion()
	}

	MouseArea {
		id: confirmDeletionMouseArea
		enabled: false
		width: listView.currentItem ? listView.currentItem.width / 4 : 0
		height: listView.currentItem ? listView.currentItem.height : 0
		x: listView.currentItem ? listView.currentItem.width - width : 0
		y: listView.currentItem ? listView.currentItem.y - listView.contentY : 0
		onClicked: performDeletion()
	}
}
