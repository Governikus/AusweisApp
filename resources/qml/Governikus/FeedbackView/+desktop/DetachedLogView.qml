/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type.LogModel
import Governikus.Type.LogFilterModel
import Governikus.View

Rectangle {
	id: root

	color: Style.color.background

	Keys.onPressed: event => {
		listView.handleKeyPress(event.key);
	}

	LogFilterModel {
		id: filterModel

	}
	ColumnLayout {
		anchors.fill: parent
		spacing: 0

		RowLayout {
			Layout.fillWidth: true
			Layout.margins: Constants.groupbox_spacing
			spacing: Constants.groupbox_spacing

			GText {
				activeFocusOnTab: true
				text: qsTr("Select log:")

				FocusFrame {
				}
			}
			GComboBox {
				Layout.preferredWidth: plugin.scaleFactor * 350
				model: LogModel.logFileNames

				onCurrentIndexChanged: LogModel.setLogFile(currentIndex)
			}
			GText {
				activeFocusOnTab: true
				text: qsTr("Font size:")

				FocusFrame {
				}
			}
			SpinBox {
				id: fontSize

				from: 8
				to: 20
				value: 12
			}
			GButton {
				id: filterButton

				property bool filter: false

				Accessible.name: filter ?
				//: LABEL DESKTOP
				qsTr("Filter. Activated.") :
				//: LABEL DESKTOP
				qsTr("Filter. Deactivated.")
				icon.source: filter ? "qrc:///images/filter_off.svg" : "qrc:///images/filter.svg"
				//: LABEL DESKTOP
				text: qsTr("Filter")
				tintIcon: true

				onClicked: filter = !filter
			}
			Item {
				Layout.fillWidth: true
			}
			GButton {
				icon.source: "qrc:///images/desktop/save_icon.svg"
				//: LABEL DESKTOP
				text: qsTr("Save log")
				tintIcon: true

				onClicked: {
					let filenameSuggestion = LogModel.createLogFileName(LogModel.getCurrentLogFileDate());
					fileDialog.selectFile(filenameSuggestion);
				}

				GFileDialog {
					id: fileDialog

					defaultSuffix: "log"
					//: LABEL DESKTOP
					nameFilters: qsTr("Logfiles (*.log)")

					//: LABEL DESKTOP
					title: qsTr("Save log")

					onAccepted: LogModel.saveCurrentLogFile(file)
				}
			}
		}
		GSeparator {
			Layout.fillWidth: true
			Layout.leftMargin: Constants.groupbox_spacing
			Layout.rightMargin: Constants.groupbox_spacing
			visible: filterButton.filter
		}
		RowLayout {
			Layout.fillWidth: true
			Layout.margins: Constants.groupbox_spacing
			spacing: Constants.groupbox_spacing
			visible: filterButton.filter

			GText {
				Layout.alignment: Qt.AlignTop
				activeFocusOnTab: true

				//: LABEL DESKTOP
				text: qsTr("Select level:")

				FocusFrame {
				}
			}
			Repeater {
				model: filterModel.levels

				delegate: GCheckBox {
					Layout.fillWidth: true
					checked: filterModel.selectedLevels.indexOf(text) !== -1
					text: modelData

					onCheckedChanged: filterModel.configureLevel(text, checked)
				}
			}
		}
		GSeparator {
			Layout.fillWidth: true
			Layout.leftMargin: Constants.groupbox_spacing
			Layout.rightMargin: Constants.groupbox_spacing
			visible: filterButton.filter
		}
		RowLayout {
			Layout.fillWidth: true
			Layout.margins: Constants.groupbox_spacing
			spacing: Constants.groupbox_spacing
			visible: filterButton.filter

			GText {
				Layout.alignment: Qt.AlignTop
				activeFocusOnTab: true

				//: LABEL DESKTOP
				text: qsTr("Select category:")

				FocusFrame {
				}
			}
			GridLayout {
				Layout.fillWidth: true
				columnSpacing: Constants.groupbox_spacing
				columns: (width + columnSpacing) / (repeater.maxItemWidth + columnSpacing)
				rowSpacing: Constants.groupbox_spacing

				GRepeater {
					id: repeater

					model: filterModel.categories

					delegate: GCheckBox {
						Layout.fillWidth: true
						checked: filterModel.selectedCategories.indexOf(text) !== -1
						text: modelData

						onCheckedChanged: filterModel.configureCategory(text, checked)
					}
				}
			}
		}
		GSeparator {
			Layout.fillWidth: true
			Layout.leftMargin: Constants.groupbox_spacing
			Layout.rightMargin: Constants.groupbox_spacing
		}
		GListView {
			id: listView

			Layout.bottomMargin: Constants.groupbox_spacing
			Layout.fillHeight: true
			Layout.fillWidth: true
			Layout.leftMargin: Constants.groupbox_spacing
			Layout.topMargin: Constants.groupbox_spacing
			activeFocusOnTab: true
			clip: true
			model: filterModel

			delegate: LogViewDelegate {
				font.pixelSize: fontSize.value
				width: listView.width - Constants.groupbox_spacing
			}

			Connections {
				function onFireNewLogMsg() {
					if (listView.atYEnd)
						listView.positionViewAtEnd();
				}

				target: LogModel
			}
			GText {
				activeFocusOnTab: true
				horizontalAlignment: Text.AlignHCenter
				//: INFO DESKTOP No log entries, placeholder text.
				text: qsTr("Currently there are no log entries matching your filter.")
				visible: listView.count === 0

				anchors {
					fill: parent
					rightMargin: Constants.groupbox_spacing
				}
				FocusFrame {
				}
			}
		}
	}
}
