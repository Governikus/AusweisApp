/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type.LogModel
import Governikus.Type.LogFilterModel
import Governikus.Type.ApplicationModel

SectionPage {
	id: root

	//: LABEL ANDROID IOS
	title: qsTr("Log")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: pop()
	}
	rightTitleBarAction: LogTitleBarControls {
		allowRemoveAll: comboBox.model.length > 1 // comboBox.count doesn't seem to update reliably

		onRemoveAll: {
			confirmationPopup.open();
		}
		onShare: pPopupPosition => {
			LogModel.shareLog(pPopupPosition);
		}
	}

	Connections {
		function onActivate() {
			logView.highlightScrollbar();
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
			Layout.preferredHeight: comboBox.height + Constants.pane_padding * 2
			color: Style.color.pane_sublevel

			GComboBox {
				id: comboBox

				Accessible.description: qsTr("Select log from list.")
				model: LogModel.logFileNames

				onCurrentIndexChanged: LogModel.setLogFile(currentIndex)

				anchors {
					left: parent.left
					leftMargin: Constants.pane_padding
					right: filterButton.left
					rightMargin: Constants.component_spacing
					top: parent.top
					topMargin: Constants.pane_padding
				}
			}
			TitleBarAction {
				id: filterButton

				property bool filter: false

				Accessible.checked: filter
				Accessible.name: qsTr("Filter")
				Accessible.role: Accessible.CheckBox
				icon: filter ? "qrc:///images/filter_off.svg" : "qrc:///images/filter.svg"
				iconTintColor: comboBox.textStyle.textColor

				onClicked: filter = !filter

				anchors {
					right: parent.right
					rightMargin: Constants.pane_padding
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

			GOptionsContainer {
				Layout.fillWidth: true
				containerPadding: Constants.pane_padding
				containerSpacing: Constants.groupbox_spacing
				//: LABEL ANDROID IOS
				title: qsTr("Filter")

				GText {
					//: LABEL ANDROID IOS
					text: qsTr("Level")
					textStyle: Style.text.subline
				}
				Grid {
					columnSpacing: Constants.groupbox_spacing
					columns: (width + columnSpacing) / (levelRepeater.maxItemWidth + columnSpacing)
					rowSpacing: Constants.groupbox_spacing
					width: parent.width

					GRepeater {
						id: levelRepeater

						model: filterModel.levels

						delegate: GCheckBox {
							checked: filterModel.selectedLevels.indexOf(text) !== -1
							text: modelData
							width: levelRepeater.maxItemWidth

							onCheckedChanged: filterModel.configureLevel(text, checked)
						}
					}
				}
				GText {
					//: LABEL ANDROID IOS
					text: qsTr("Category")
					textStyle: Style.text.subline
				}
				Grid {
					columnSpacing: Constants.groupbox_spacing
					columns: (width + columnSpacing) / (categoryRepeater.maxItemWidth + columnSpacing)
					rowSpacing: Constants.groupbox_spacing
					width: parent.width

					GRepeater {
						id: categoryRepeater

						model: filterModel.categories

						delegate: GCheckBox {
							checked: filterModel.selectedCategories.indexOf(text) !== -1
							text: modelData
							width: categoryRepeater.maxItemWidth

							onCheckedChanged: filterModel.configureCategory(text, checked)
						}
					}
				}
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
				visible: logView.count === 0
				width: parent.width - 2 * Constants.component_spacing
			}
		}
	}
	ConfirmationPopup {
		id: confirmationPopup

		//: LABEL ANDROID IOS
		okButtonText: qsTr("Delete")
		//: INFO ANDROID IOS All logfiles are about to be removed, user confirmation required.
		text: qsTr("All old logs will be deleted.")
		//: LABEL ANDROID IOS
		title: qsTr("Delete all logs")

		onConfirmed: LogModel.removeOtherLogFiles()
	}
}
