/**
 * Copyright (c) 2020-2026 Governikus GmbH & Co. KG, Germany
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
		listView.handleKeyPress(event);
	}

	LogModel {
		id: logModel

	}
	LogFilterModel {
		id: filterModel

		sourceModel: logModel
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

			GText {
				Accessible.role: Accessible.Heading
				Layout.alignment: Qt.AlignBottom
				//: DESKTOP
				text: qsTr("Current Log")
				textStyle: Style.text.title
			}
			GSpacer {
				Layout.fillWidth: true
			}
			ColumnLayout {
				GText {
					//: DESKTOP
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
				//: DESKTOP
				qsTr("Filter. Activated.") :
				//: DESKTOP
				qsTr("Filter. Deactivated.")
				icon.source: filter ? "qrc:///images/filter_off.svg" : "qrc:///images/filter.svg"
				//: DESKTOP
				text: qsTr("Filter")

				onClicked: filter = !filter
			}
			LogButton {
				icon.source: "qrc:///images/desktop/save_icon.svg"
				//: DESKTOP
				text: qsTr("Save log")

				onClicked: {
					let filenameSuggestion = logModel.createLogFileName();
					fileDialog.selectFile(filenameSuggestion);
				}

				GFileDialog {
					id: fileDialog

					defaultSuffix: "log"
					//: DESKTOP
					nameFilters: qsTr("Logfiles (*.log)")

					//: DESKTOP
					title: qsTr("Save log")

					onAccepted: logModel.saveLogFile(selectedFile, true)
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

				//: DESKTOP
				text: qsTr("Select level:")
				textStyle: logTextStyle
			}
			Repeater {
				model: filterModel.levels

				delegate: GCheckBox {
					required property string modelData

					Layout.fillWidth: false
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

				//: DESKTOP
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
			clip: true
			model: filterModel

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

				target: logModel
			}
			GText {
				horizontalAlignment: Text.AlignHCenter
				//: DESKTOP No log entries, placeholder text.
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
