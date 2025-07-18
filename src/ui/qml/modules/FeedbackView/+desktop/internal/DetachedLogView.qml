/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.Type

Rectangle {
	id: root

	readonly property int controlRadius: 15
	readonly property int horizontalPadding: 18
	readonly property int spacing: 10
	readonly property int verticalPadding: 6

	color: Style.color.background

	Keys.onPressed: event => {
		listView.handleKeyPress(event.key);
	}

	LogFilterModel {
		id: filterModel

	}
	LogTextStyle {
		id: logTextStyle

	}
	ColumnLayout {
		anchors.fill: parent
		spacing: 0

		RowLayout {
			Layout.fillWidth: true
			Layout.margins: root.spacing
			spacing: root.spacing

			ColumnLayout {
				GText {
					//: LABEL DESKTOP
					text: qsTr("Select log:")
					textStyle: logTextStyle
				}
				GComboBox {
					Layout.preferredWidth: 200
					horizontalPadding: root.horizontalPadding
					model: LogModel.logFileNames
					radius: root.controlRadius
					textStyle: logTextStyle
					verticalPadding: root.verticalPadding

					background: GPaneBackground {
						border.color: logTextStyle.textColor
						border.width: 1
						color: Style.color.transparent
						drawShadow: false
						radius: root.controlRadius
					}

					onCurrentIndexChanged: LogModel.setLogFile(currentIndex)
				}
			}
			ColumnLayout {
				GText {
					//: LABEL DESKTOP
					text: qsTr("Zoom:")
					textStyle: logTextStyle
				}
				SpinBox {
					id: zoomBox

					from: 50
					stepSize: 10
					textFromValue: function (value) {
						return value + " %";
					}
					to: 200
					value: 100
				}
			}
			LogButton {
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

				onClicked: filter = !filter
			}
			Item {
				Layout.fillWidth: true
			}
			LogButton {
				icon.source: "qrc:///images/desktop/save_icon.svg"
				//: LABEL DESKTOP
				text: qsTr("Save log")

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
			Layout.leftMargin: root.spacing
			Layout.rightMargin: root.spacing
			implicitHeight: 1
			visible: filterButton.filter
		}
		RowLayout {
			Layout.fillWidth: true
			Layout.margins: root.spacing
			spacing: root.spacing
			visible: filterButton.filter

			GText {
				Layout.alignment: Qt.AlignTop

				//: LABEL DESKTOP
				text: qsTr("Select level:")
				textStyle: logTextStyle
			}
			Repeater {
				model: filterModel.levels

				delegate: GCheckBox {
					required property string modelData

					Layout.fillWidth: true
					checked: filterModel.selectedLevels.indexOf(text) !== -1
					horizontalPadding: 0
					text: modelData
					textStyle: logTextStyle
					verticalPadding: 0

					onCheckedChanged: filterModel.configureLevel(text, checked)
				}
			}
		}
		GSeparator {
			Layout.fillWidth: true
			Layout.leftMargin: root.spacing
			Layout.rightMargin: root.spacing
			implicitHeight: 1
			visible: filterButton.filter
		}
		RowLayout {
			Layout.fillWidth: true
			Layout.margins: root.spacing
			spacing: root.spacing
			visible: filterButton.filter

			GText {
				Layout.alignment: Qt.AlignTop

				//: LABEL DESKTOP
				text: qsTr("Select category:")
			}
			GridLayout {
				Layout.fillWidth: true
				columnSpacing: root.spacing
				columns: (width + columnSpacing) / (repeater.maxItemWidth + columnSpacing)
				rowSpacing: root.spacing

				GRepeater {
					id: repeater

					model: filterModel.categories

					delegate: GCheckBox {
						required property string modelData

						Layout.fillWidth: true
						checked: filterModel.selectedCategories.indexOf(text) !== -1
						horizontalPadding: 0
						text: modelData
						textStyle: logTextStyle
						verticalPadding: 0

						onCheckedChanged: filterModel.configureCategory(text, checked)
					}
				}
			}
		}
		GSeparator {
			Layout.fillWidth: true
			Layout.leftMargin: root.spacing
			Layout.rightMargin: root.spacing
			implicitHeight: 1
		}
		GListView {
			id: listView

			Layout.bottomMargin: root.spacing
			Layout.fillHeight: true
			Layout.fillWidth: true
			Layout.leftMargin: root.spacing
			Layout.topMargin: root.spacing
			activeFocusOnTab: true
			clip: true
			model: filterModel

			ScrollBar.vertical: GScrollBar {
				borderWidth: 1
				bottomPadding: 0
				implicitWidth: 6 + leftPadding + rightPadding
				rightPadding: 3
				topPadding: 0
			}
			delegate: LogViewDelegate {
				required property int index

				font.pixelSize: 0.12 * zoomBox.value
				width: listView.width - root.spacing

				onActiveFocusChanged: if (activeFocus) {
					listView.handleItemFocused(index);
				}
			}

			Connections {
				function onFireNewLogMsg() {
					if (listView.atYEnd)
						listView.positionViewAtEnd();
				}

				target: LogModel
			}
			GText {
				horizontalAlignment: Text.AlignHCenter
				//: INFO DESKTOP No log entries, placeholder text.
				text: qsTr("Currently there are no log entries matching your filter.")
				visible: listView.count === 0

				anchors {
					fill: parent
					rightMargin: root.spacing
				}
			}
		}
	}

	component LogButton: GButton {
		Layout.alignment: Qt.AlignBottom
		Layout.minimumWidth: -1
		borderWidth: 1
		horizontalPadding: root.horizontalPadding
		radius: root.controlRadius
		spacing: root.spacing
		tintIcon: true
		verticalPadding: root.verticalPadding

		textStyle: LogTextStyle {
		}
	}
	component LogTextStyle: TextStyle {
		lineHeight: 16
		textSize: 16
	}
}
