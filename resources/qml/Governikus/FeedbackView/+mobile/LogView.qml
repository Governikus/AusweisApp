/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.LogModel 1.0


SectionPage
{
	id: root
	navigationAction: NavigationAction { state: "back"; onClicked: firePop() }
	//: LABEL ANDROID IOS
	title: qsTr("Log") + SettingsModel.translationTrigger
	rightTitleBarAction: LogTitleBarControls {
		allowRemove: comboBox.currentIndex > 0
		allowRemoveAll: comboBox.model.length > 1 // comboBox.count doesn't seem to update reliably

		onShare: LogModel.shareLog(popupPosition)

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

	Item {
		id: logSelector
		height: comboBox.height
		width: parent.width - 2 * Constants.pane_padding
		anchors.top: parent.top
		anchors.topMargin: Constants.pane_padding
		anchors.horizontalCenter: parent.horizontalCenter

		GText {
			id: comboText
			anchors.left: parent.left
			anchors.verticalCenter: parent.verticalCenter

			//: LABEL ANDROID IOS
			text: qsTr("Log:") + SettingsModel.translationTrigger
		}

		GComboBox {
			id: comboBox

			width: parent.width - comboText.width - Constants.component_spacing
			anchors.right: parent.right

			Accessible.description: qsTr("Select logfile from list.") + SettingsModel.translationTrigger

			model: LogModel.logFiles
			onActivated: LogModel.setLogfile(comboBox.currentIndex)
		}
	}

	Pane {
		id: pane

		anchors {
			top: logSelector.bottom
			bottom: parent.bottom
			left: parent.left
			right: parent.right
			margins: Constants.pane_padding
		}

		clip: true

		GListView {
			id: logView

			height: pane.height - 2 * Constants.pane_padding
			width: pane.width - Constants.pane_padding
			displayMarginBeginning: Constants.pane_padding
			displayMarginEnd: Constants.pane_padding

			model: LogModel
			spacing: Constants.text_spacing
			delegate: Item {
				height: layout.height
				width: logView.width - Constants.pane_padding

				Accessible.role: Accessible.StaticText
				Accessible.name: display
				Accessible.onScrollDownAction: logView.scrollPageDown()
				Accessible.onScrollUpAction: logView.scrollPageUp()

				ColumnLayout {
					id: layout

					width: parent.width

					spacing: 0

					GText {
						Layout.fillWidth: true

						Accessible.ignored: true

						text: origin
						textStyle: Style.text.hint
						font.bold: true
					}

					GText {
						Layout.fillWidth: true

						Accessible.ignored: true

						text: message
						textStyle: Style.text.hint
					}
				}
			}

			Connections {
				target: LogModel
				onFireNewLogMsg: if (logView.atYEnd) logView.positionViewAtEnd()
			}
		}
	}

	ConfirmationPopup {
		id: confirmationPopup

		property bool deleteAll: true

		title: (deleteAll ?
				//: LABEL ANDROID IOS
				qsTr("Delete all") :
				//: LABEL ANDROID IOS
				qsTr("Delete")
			   ) + SettingsModel.translationTrigger
		//: INFO ANDROID IOS All logfiles are about to be removed, user confirmation required.
		text: (deleteAll ? qsTr("All old logfiles will be deleted.")
						 //: INFO ANDROID IOS The current logfile is about to be removed, user confirmation required.
						 : qsTr("The logfile will be deleted.")
			   ) + SettingsModel.translationTrigger
		//: LABEL ANDROID IOS
		okButtonText: qsTr("Delete") + SettingsModel.translationTrigger
		onConfirmed: deleteAll ? LogModel.removeOtherLogfiles() : LogModel.removeCurrentLogfile()
	}
}
