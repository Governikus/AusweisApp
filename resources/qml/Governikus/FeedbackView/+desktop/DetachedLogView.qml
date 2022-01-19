/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.LogModel 1.0
import Governikus.View 1.0


Item {
	id: root

	Keys.onPressed: listView.handleKeyPress(event.key)

	ColumnLayout {
		anchors.fill: parent

		spacing: 0

		RowLayout {
			Layout.fillWidth: true
			Layout.margins: Constants.groupbox_spacing

			spacing: Constants.groupbox_spacing

			GText {
				text: qsTr("Select log:")
				textStyle: Style.text.normal
			}

			GComboBox {
				Layout.preferredWidth: ApplicationModel.scaleFactor * 350

				model: LogModel.logFiles

				onCurrentIndexChanged: LogModel.setLogFile(currentIndex)
			}

			GText {
				text: qsTr("Font size:")
				textStyle: Style.text.normal
			}

			SpinBox {
				id: fontSize

				from: 8
				to: 20
				value: 10
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
					appWindow.openSaveFileDialog(LogModel.saveCurrentLogFile, filenameSuggestion, qsTr("Logfiles"), "log")
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

			clip: true
			scrollBarEnabled: true
			model: LogModel
			delegate: LogViewDelegate {
				width: listView.width - Constants.groupbox_spacing
				pixelSize: fontSize.value
			}

			Connections {
				target: LogModel
				onFireNewLogMsg: if (listView.atYEnd) listView.positionViewAtEnd()
			}
		}
	}
}
