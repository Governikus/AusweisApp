/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.LogModel 1.0
import Governikus.Type.LogFilterModel 1.0
import Governikus.View 1.0


Item {
	id: root

	Keys.onPressed: event => { listView.handleKeyPress(event.key) }

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
				textStyle: Style.text.normal

				FocusFrame {}
			}

			GComboBox {
				Layout.preferredWidth: ApplicationModel.scaleFactor * 350

				model: LogModel.logFileNames

				onCurrentIndexChanged: LogModel.setLogFile(currentIndex)
			}

			GText {
				activeFocusOnTab: true

				text: qsTr("Font size:")
				textStyle: Style.text.normal

				FocusFrame {}
			}

			SpinBox {
				id: fontSize

				from: 8
				to: 20
				value: 10
			}

			GButton {
				id: filterButton

				property bool filter: false

				Accessible.name: filter
								 //: LABEL DESKTOP
								 ? qsTr("Filter. Activated.")
								 //: LABEL DESKTOP
								 : qsTr("Filter. Deactivated.")

				icon.source: filter ? "qrc:///images/material_filter_off.svg" : "qrc:///images/material_filter.svg"
				//: LABEL DESKTOP
				text: qsTr("Filter")
				tintIcon: true

				onClicked: filter = !filter
			}

			Item {
				Layout.fillWidth: true
			}

			GButton {
				icon.source: "qrc:///images/desktop/material_save.svg"
				//: LABEL DESKTOP
				text: qsTr("Save log")
				tintIcon: true

				onClicked: {
					let filenameSuggestion = LogModel.createLogFileName(LogModel.getCurrentLogFileDate())
					fileDialog.selectFile(filenameSuggestion)
				}

				GFileDialog {
					id: fileDialog

					//: LABEL DESKTOP
					title: qsTr("Save log")
					defaultSuffix: "log"
					//: LABEL DESKTOP
					nameFilters: qsTr("Logfiles (*.log)")

					onAccepted: LogModel.saveCurrentLogFile(file)
				}
			}
		}

		GSeparator {
			visible: filterButton.filter
			Layout.fillWidth: true
			Layout.leftMargin: Constants.groupbox_spacing
			Layout.rightMargin: Constants.groupbox_spacing
		}

		RowLayout {
			visible: filterButton.filter
			Layout.fillWidth: true
			Layout.margins: Constants.groupbox_spacing

			spacing: Constants.groupbox_spacing

			GText {
				Layout.alignment: Qt.AlignTop

				activeFocusOnTab: true

				//: LABEL DESKTOP
				text: qsTr("Select level:")
				textStyle: Style.text.normal

				FocusFrame {}
			}

			Repeater {
				model: filterModel.levels
				delegate: ToggleableOption {
					Layout.fillWidth: true

					checked: filterModel.selectedLevels.indexOf(text) !== -1
					text: modelData

					onCheckedChanged: filterModel.configureLevel(text, checked)
				}
			}
		}

		GSeparator {
			visible: filterButton.filter
			Layout.fillWidth: true
			Layout.leftMargin: Constants.groupbox_spacing
			Layout.rightMargin: Constants.groupbox_spacing
		}


		RowLayout {
			visible: filterButton.filter
			Layout.fillWidth: true
			Layout.margins: Constants.groupbox_spacing

			spacing: Constants.groupbox_spacing

			GText {
				Layout.alignment: Qt.AlignTop

				activeFocusOnTab: true

				//: LABEL DESKTOP
				text: qsTr("Select category:")
				textStyle: Style.text.normal

				FocusFrame {}
			}

			GridLayout {
				Layout.fillWidth: true

				columns: (width + columnSpacing) / (repeater.maxItemWidth + columnSpacing)
				columnSpacing: Constants.groupbox_spacing
				rowSpacing: Constants.groupbox_spacing

				GRepeater {
					id: repeater

					model: filterModel.categories
					delegate: ToggleableOption {
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

			Layout.fillHeight: true
			Layout.fillWidth: true
			Layout.leftMargin: Constants.groupbox_spacing
			Layout.bottomMargin: Constants.groupbox_spacing
			Layout.topMargin: Constants.groupbox_spacing

			activeFocusOnTab: true
			clip: true
			scrollBarEnabled: true
			model: filterModel
			delegate: LogViewDelegate {
				width: listView.width - Constants.groupbox_spacing

				font.pixelSize: fontSize.value
			}

			highlight: LogViewHighLight {
				currentItem: listView.currentItem

				textSize: fontSize.value
			}

			Connections {
				target: LogModel
				function onFireNewLogMsg() { if (listView.atYEnd) listView.positionViewAtEnd() }
			}

			GText {
				visible: listView.count === 0
				width: Math.min(parent.width, contentWidth)
				anchors.centerIn: parent

				activeFocusOnTab: true

				horizontalAlignment: Text.AlignHCenter
				//: INFO DESKTOP No log entries, placeholder text.
				text: qsTr("Currently there are no log entries matching your filter.")
				textStyle: Style.text.normal_secondary

				FocusFrame {}
			}
		}
	}
}
