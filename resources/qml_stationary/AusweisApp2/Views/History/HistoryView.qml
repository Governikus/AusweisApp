import AusweisApp2.Global 1.0

import QtQml 2.2
import QtQuick 2.7
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3


Item {
	id: baseItem

	Column {
		id: header
		width: parent.width
		spacing: Constants.pane_spacing

		Text {
			width: parent.width
			wrapMode: Text.WordWrap
			text: qsTr("This page displays the history of your successful authentications. " +
					   "Double-click on a service provider for more information. " +
					   "You can delete parts or the entire history. " +
					   "You can also save the history as a PDF file.")
		}

		Item {
			height: searchField.height
			width: parent.width

			Text {
				id: searchLabel
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Search:") + settingsModel.translationTrigger
				textFormat: Text.StyledText
			}
			TextField {
				id: searchField
				anchors.left: searchLabel.right
				anchors.leftMargin: Constants.pane_spacing
				anchors.right: parent.right
				onTextChanged: historyModel.searchFilter.setFilterString(text)
			}
		}
	}

	Item {
		anchors.top: header.bottom
		anchors.topMargin: header.spacing
		anchors.bottom: footer.top
		anchors.bottomMargin: header.spacing
		width: parent.width

		Text {
			anchors.centerIn: parent
			text: qsTr("No history entry available") + settingsModel.translationTrigger
			wrapMode: Text.WordWrap
			font.pixelSize: Constants.normal_font_size
			visible: !scrollView.visible
		}

		ScrollView {
			id: scrollView
			width: parent.width
			anchors.fill: parent
			anchors.topMargin: 1
			anchors.bottomMargin: 1
			clip: true
			horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
			verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn
			visible: listView.count > 0

			Rectangle {
				width: baseItem.width
				height: listView.contentHeight
				color: PlatformConstants.grey_border

			}

			ListView {
				id: listView
				width: parent.width
				spacing: 1
				model: historyModel.searchFilter

				delegate: ListViewDelegate {
					width: parent.width
					height: Utils.dp(120)
					historyModelItem: model
					onShowHistoryDetails:widgetPlugin.showDetailDialog(model.termsOfUsage)
				}
			}
		}

		Rectangle {
			anchors.top: parent.top
			width: parent.width
			height: 1
			color: PlatformConstants.grey_border
		}
		Rectangle {
			anchors.bottom: parent.bottom
			anchors.bottomMargin: height
			width: parent.width
			height: 1
			color: PlatformConstants.grey_border
		}
	}

	Rectangle {
		id: footer
		width: parent.width
		height: Utils.dp(50)
		anchors.bottom: parent.bottom

		RowLayout {
			anchors.fill: parent
			spacing: Constants.pane_spacing

			Text {
				text: qsTr("History:") + settingsModel.translationTrigger
			}

			CheckBox {
				checked: historyModel.enabled
				onCheckedChanged: historyModel.enabled = checked
			}

			Item {
				// Quickfix because CheckBox text turns into a black box sometimes
				// TODO: Use text property from checkbox
				height: childrenRect.height
				width: childrenRect.width

				Text {
					anchors.left: parent.left
					anchors.leftMargin: -Constants.pane_spacing
					text: qsTr("save") + settingsModel.translationTrigger
				}
			}

			Item {
				Layout.fillWidth: true
			}

			Button {
				text: qsTr("Delete History") + settingsModel.translationTrigger
				enabled:  listView.count > 0
				onClicked: deleteConfirmationDialog.open()
			}

			Button {
				text: qsTr("Save as PDF") + settingsModel.translationTrigger
				enabled:  listView.count > 0
				onClicked: fileDialog.open()
			}
		}
	}


	MessageDialog {
		id: deleteConfirmationDialog
		icon: StandardIcon.Question
		modality: Qt.ApplicationModal
		standardButtons: StandardButton.Yes | StandardButton.No
		title: qsTr("Delete history") + settingsModel.translationTrigger
		text: qsTr("Do you really want to delete the history?") + settingsModel.translationTrigger
		onYes: { historyModel.removeRows(0, historyModel.rowCount()) }
	}
	FileDialog {
		id: fileDialog
		modality: Qt.ApplicationModal
		selectExisting: false
		title: Qt.application.name + " - " + qsTr("Save history") + settingsModel.translationTrigger
		folder: shortcuts.home
		nameFilters: [qsTr("PDF Documents (*.pdf)") + settingsModel.translationTrigger]
		onAccepted: {
			qmlExtension.exportHistory(fileDialog.fileUrls)
		}
	}
}
