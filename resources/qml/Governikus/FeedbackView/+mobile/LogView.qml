/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.LogModel 1.0
import Governikus.Type.LogFilterModel 1.0
import Governikus.Type.ApplicationModel 1.0


SectionPage {
	id: root

	navigationAction: NavigationAction { action: NavigationAction.Action.Back; onClicked: pop() }
	//: LABEL ANDROID IOS
	title: qsTr("Log")
	rightTitleBarAction: LogTitleBarControls {
		allowRemove: comboBox.currentIndex > 0
		allowRemoveAll: comboBox.model.length > 1 // comboBox.count doesn't seem to update reliably

		onShare: pPopupPosition => { LogModel.shareLog(pPopupPosition) }

		onRemove: {
			confirmationPopup.deleteAll = false
			confirmationPopup.open()
		}

		onRemoveAll: {
			confirmationPopup.deleteAll = true
			confirmationPopup.open()
		}
	}

	sectionPageFlickable: logView

	LogFilterModel {
		id: filterModel
	}

	ColumnLayout {
		anchors.fill: parent

		spacing: filterButton.filter ? Constants.text_spacing : 0

		Rectangle {
			id: logSelector

			Layout.fillWidth: true
			Layout.preferredHeight: comboBox.height + Constants.text_spacing

			color: Style.color.accent

			GComboBox {
				id: comboBox

				anchors {
					left: parent.left
					leftMargin: Constants.text_spacing
					right: filterButton.left
					rightMargin: Constants.text_spacing
					top: parent.top
				}

				Accessible.description: qsTr("Select log from list.")

				model: LogModel.logFileNames
				onCurrentIndexChanged: LogModel.setLogFile(currentIndex)
			}

			TitleBarButton {
				id: filterButton

				property bool filter: false

				anchors {
					right: parent.right
					margins: Constants.text_spacing
					verticalCenter: comboBox.verticalCenter
				}

				Accessible.role: Accessible.CheckBox
				Accessible.checked: filter
				Accessible.name: qsTr("Filter")

				source: filter ? "qrc:///images/material_filter_off.svg" : "qrc:///images/material_filter.svg"
				onClicked: filter = !filter
			}

		}


		TitledSeparator {
			visible: filterButton.filter
			Layout.preferredWidth: parent.width

			contentMarginTop: 0

			//: LABEL ANDROID IOS
			title: qsTr("Filter")
		}

		GText {
			visible: filterButton.filter
			Layout.leftMargin: Constants.component_spacing

			//: LABEL ANDROID IOS
			text: qsTr("Select level:")
			textStyle: Style.text.normal_accent
		}

		GridLayout {
			visible: filterButton.filter
			Layout.fillWidth: true
			Layout.leftMargin: Constants.component_spacing

			columns: (width + columnSpacing) / (levelRepeater.maxItemWidth + columnSpacing)
			columnSpacing: Constants.groupbox_spacing
			rowSpacing: Constants.groupbox_spacing

			GRepeater {
				id: levelRepeater

				model: filterModel.levels
				delegate: GCheckBox {
					Layout.fillWidth: true

					checked: filterModel.selectedLevels.indexOf(text) !== -1
					text: modelData

					onToggled: filterModel.configureLevel(text, checked)
				}
			}
		}

		GText {
			visible: filterButton.filter
			Layout.leftMargin: Constants.component_spacing

			//: LABEL ANDROID IOS
			text: qsTr("Select category:")
			textStyle: Style.text.normal_accent
		}

		GridLayout {
			visible: filterButton.filter
			Layout.fillWidth: true
			Layout.leftMargin: Constants.component_spacing

			columns: (width + columnSpacing) / (categoryRepeater.maxItemWidth + columnSpacing)
			columnSpacing: Constants.groupbox_spacing
			rowSpacing: Constants.groupbox_spacing

			GRepeater {
				id: categoryRepeater

				model: filterModel.categories
				delegate: GCheckBox {
					Layout.fillWidth: true

					checked: filterModel.selectedCategories.indexOf(text) !== -1
					text: modelData

					onToggled: filterModel.configureCategory(text, checked)
				}
			}
		}

		GListView {
			id: logView

			Layout.fillWidth: true
			Layout.fillHeight: true

			clip: true
			model: filterModel
			delegate: ListItem {
				readonly property bool isLastItem: index === ListView.view.count - 1

				Accessible.onScrollDownAction: ListView.view.scrollPageDown()
				Accessible.onScrollUpAction: ListView.view.scrollPageUp()

				showLinkIcon: false
				showSeparator: !isLastItem
				fixedHeight: false
				headerText: origin
				text: message

				onPressAndHold: {
					ApplicationModel.setClipboardText(display)
					//: INFO ANDROID IOS Toast message used to confirm the copy of a log entry.
					ApplicationModel.showFeedback(qsTr("The log entry was copied to the clipboard."))
				}
			}

			Connections {
				target: LogModel
				function onFireNewLogMsg() { if (logView.atYEnd) logView.positionViewAtEnd() }
			}

			GText {
				visible: logView.count === 0
				width: parent.width - 2 * Constants.component_spacing
				anchors.centerIn: parent

				horizontalAlignment: Text.AlignHCenter
				//: INFO ANDROID IOS No log entries, placeholder text.
				text: qsTr("Currently there are no log entries matching your filter.")
				textStyle: Style.text.normal_secondary
			}
		}
	}

	ConfirmationPopup {
		id: confirmationPopup

		property bool deleteAll: true

		title: (deleteAll ?
				//: LABEL ANDROID IOS
				qsTr("Delete all logs") :
				//: LABEL ANDROID IOS
				qsTr("Delete log")
			   )
		//: INFO ANDROID IOS All logfiles are about to be removed, user confirmation required.
		text: (deleteAll ? qsTr("All old logs will be deleted.")
						 //: INFO ANDROID IOS The current logfile is about to be removed, user confirmation required.
						 : qsTr("The log will be deleted.")
			   )
		//: LABEL ANDROID IOS
		okButtonText: qsTr("Delete")
		onConfirmed: deleteAll ? LogModel.removeOtherLogFiles() : LogModel.removeCurrentLogFile()
	}
}
