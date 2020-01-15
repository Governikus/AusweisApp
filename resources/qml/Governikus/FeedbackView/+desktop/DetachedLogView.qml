/*
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.LogModel 1.0
import Governikus.Type.SettingsModel 1.0
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
				text: qsTr("Select logfile:") + SettingsModel.translationTrigger
				textStyle: Style.text.normal
			}

			GComboBox {
				Layout.preferredWidth: ApplicationModel.scaleFactor * 350

				model: LogModel.logFiles

				onCurrentIndexChanged: LogModel.setLogfile(currentIndex)
			}

			GText {
				text: qsTr("Font size:") + SettingsModel.translationTrigger
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
				icon.source: "qrc:///images/icon_save.svg"
				//: LABEL DESKTOP_QML
				text: qsTr("Save logfile") + SettingsModel.translationTrigger
				tintIcon: true

				onClicked: {
					let filenameSuggestion = LogModel.createLogFileName(LogModel.getCurrentLogfileDate())
					appWindow.openSaveFileDialog(LogModel.saveCurrentLogfile, filenameSuggestion, "log")
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
