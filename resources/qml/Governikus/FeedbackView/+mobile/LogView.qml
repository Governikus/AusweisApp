/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
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
	sectionPageFlickable: logView
	//: LABEL ANDROID IOS
	title: qsTr("Log")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: pop()
	}
	rightTitleBarAction: LogTitleBarControls {
		allowRemove: comboBox.currentIndex > 0
		allowRemoveAll: comboBox.model.length > 1 // comboBox.count doesn't seem to update reliably

		onRemove: {
			confirmationPopup.deleteAll = false;
			confirmationPopup.open();
		}
		onRemoveAll: {
			confirmationPopup.deleteAll = true;
			confirmationPopup.open();
		}
		onShare: pPopupPosition => {
			LogModel.shareLog(pPopupPosition);
		}
	}

	LogFilterModel {
		id: filterModel
	}
	ColumnLayout {
		anchors.fill: parent
		spacing: 0

		Rectangle {
			id: logSelector
			Layout.fillWidth: true
			Layout.preferredHeight: comboBox.height + Constants.text_spacing
			color: Style.color.accent

			GComboBox {
				id: comboBox
				Accessible.description: qsTr("Select log from list.")
				model: LogModel.logFileNames

				onCurrentIndexChanged: LogModel.setLogFile(currentIndex)

				anchors {
					left: parent.left
					leftMargin: Constants.text_spacing
					right: filterButton.left
					rightMargin: Constants.text_spacing
					top: parent.top
				}
			}
			TitleBarButton {
				id: filterButton

				property bool filter: false

				Accessible.checked: filter
				Accessible.name: qsTr("Filter")
				Accessible.role: Accessible.CheckBox
				source: filter ? "qrc:///images/material_filter_off.svg" : "qrc:///images/material_filter.svg"

				onClicked: filter = !filter

				anchors {
					margins: Constants.text_spacing
					right: parent.right
					verticalCenter: comboBox.verticalCenter
				}
			}
		}
		GFlickableColumnLayout {
			Layout.fillHeight: true
			Layout.fillWidth: true
			clip: true
			spacing: Constants.text_spacing
			visible: filterButton.filter

			TitledSeparator {
				Layout.fillWidth: true
				contentMarginLeft: 0
				contentMarginRight: 0
				contentMarginTop: 0

				//: LABEL ANDROID IOS
				title: qsTr("Filter")
			}
			GText {
				//: LABEL ANDROID IOS
				text: qsTr("Level")
				textStyle: Style.text.normal_accent
			}
			GridLayout {
				Layout.fillWidth: true
				columnSpacing: Constants.groupbox_spacing
				columns: (width + columnSpacing) / (levelRepeater.maxItemWidth + columnSpacing)
				rowSpacing: Constants.groupbox_spacing

				GRepeater {
					id: levelRepeater
					model: filterModel.levels

					delegate: GCheckBox {
						Layout.fillWidth: true
						checked: filterModel.selectedLevels.indexOf(text) !== -1
						text: modelData

						onCheckedChanged: filterModel.configureLevel(text, checked)
					}
				}
			}
			GText {
				//: LABEL ANDROID IOS
				text: qsTr("Category")
				textStyle: Style.text.normal_accent
			}
			GridLayout {
				Layout.fillWidth: true
				columnSpacing: Constants.groupbox_spacing
				columns: (width + columnSpacing) / (categoryRepeater.maxItemWidth + columnSpacing)
				rowSpacing: Constants.groupbox_spacing

				GRepeater {
					id: categoryRepeater
					model: filterModel.categories

					delegate: GCheckBox {
						Layout.fillWidth: true
						checked: filterModel.selectedCategories.indexOf(text) !== -1
						text: modelData

						onCheckedChanged: filterModel.configureCategory(text, checked)
					}
				}
			}
			GSpacer {
				Layout.fillHeight: true
			}
		}
		GListView {
			id: logView
			Layout.fillHeight: true
			Layout.fillWidth: true
			clip: true
			model: filterModel
			visible: !filterButton.filter

			delegate: ListItem {
				readonly property bool isLastItem: index === ListView.view.count - 1

				fixedHeight: false
				headerText: origin
				showLinkIcon: false
				showSeparator: !isLastItem
				text: message

				Accessible.onScrollDownAction: ListView.view.scrollPageDown()
				Accessible.onScrollUpAction: ListView.view.scrollPageUp()
				onPressAndHold: {
					ApplicationModel.setClipboardText(display);
					//: INFO ANDROID IOS Toast message used to confirm the copy of a log entry.
					ApplicationModel.showFeedback(qsTr("The log entry was copied to the clipboard."));
				}
			}

			Connections {
				function onFireNewLogMsg() {
					if (logView.atYEnd)
						logView.positionViewAtEnd();
				}

				target: LogModel
			}
			GText {
				anchors.centerIn: parent
				horizontalAlignment: Text.AlignHCenter
				//: INFO ANDROID IOS No log entries, placeholder text.
				text: qsTr("Currently there are no log entries matching your filter.")
				textStyle: Style.text.normal_secondary
				visible: logView.count === 0
				width: parent.width - 2 * Constants.component_spacing
			}
		}
	}
	ConfirmationPopup {
		id: confirmationPopup

		property bool deleteAll: true

		//: LABEL ANDROID IOS
		okButtonText: qsTr("Delete")
		//: INFO ANDROID IOS All logfiles are about to be removed, user confirmation required.
		text: (deleteAll ? qsTr("All old logs will be deleted.") :
			//: INFO ANDROID IOS The current logfile is about to be removed, user confirmation required.
			qsTr("The log will be deleted."))
		title: (deleteAll ?
			//: LABEL ANDROID IOS
			qsTr("Delete all logs") :
			//: LABEL ANDROID IOS
			qsTr("Delete log"))

		onConfirmed: deleteAll ? LogModel.removeOtherLogFiles() : LogModel.removeCurrentLogFile()
	}
}
