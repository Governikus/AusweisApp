/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.LogModel 1.0


SectionPage
{
	id: root

	Accessible.name: qsTr("Logfile viewer") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the logfile viewer of the AusweisApp2.") + SettingsModel.translationTrigger

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("Application log") + SettingsModel.translationTrigger
	}

	TabbedPane {
		id: tabbedPane

		anchors.fill: parent
		anchors.margins: Constants.pane_padding

		sectionsModel: LogModel.logFiles
		onCurrentIndexChanged: LogModel.setLogfile(currentIndex)

		contentDelegate: logSectionDelegate

		footerItem: Item {
			height: buttonLayout.implicitHeight

			ColumnLayout {
				id: buttonLayout

				anchors.fill: parent
				anchors.rightMargin: Constants.groupbox_spacing
				spacing: Constants.groupbox_spacing

				GButton {
					id: saveLog

					Layout.fillWidth: true
					activeFocusOnTab: true

					icon.source: "qrc:///images/icon_save.svg"
					//: LABEL DESKTOP_QML
					text: qsTr("Save logfile") + SettingsModel.translationTrigger
					enabled: tabbedPane.sectionsModel.length > 0
					onClicked: {
						var creationDate = LogModel.getCurrentLogfileDate().toLocaleString(Qt.locale(), "yyyy-MM-dd_HH-mm")
						var filenameSuggestion = "%1.%2.log".arg(Qt.application.name).arg(creationDate)
						appWindow.openSaveFileDialog(LogModel.saveCurrentLogfile, filenameSuggestion, "log")
					}
				}

				GButton {
					id: removeLog

					Layout.fillWidth: true
					activeFocusOnTab: true

					icon.source: "qrc:///images/trash_icon_white.svg"
					//: LABEL DESKTOP_QML
					text: qsTr("Delete logfile") + SettingsModel.translationTrigger
					enabled: tabbedPane.currentIndex > 0
					onClicked: {
						confirmationPopup.deleteAll = false
						confirmationPopup.open()
					}
				}

				GButton {
					id: removeAllLogs

					Layout.fillWidth: true
					activeFocusOnTab: true

					icon.source: "qrc:///images/trash_icon_all.svg"
					//: LABEL DESKTOP_QML
					text: qsTr("Delete old logfiles") + SettingsModel.translationTrigger
					enabled: tabbedPane.sectionsModel.length > 1
					onClicked: {
						confirmationPopup.deleteAll = true
						confirmationPopup.open()
					}
				}
			}
		}
	}

	Component {
		id: logSectionDelegate

		Item {
			height: tabbedPane.availableHeight
			width: parent.width

			ListView {
				id: logView

				anchors.fill: parent

				activeFocusOnTab: true

				displayMarginBeginning: Constants.pane_padding
				displayMarginEnd: Constants.pane_padding
				boundsBehavior: Flickable.StopAtBounds
				model: LogModel
				delegate: GText {
					width: parent.width - 2 * Constants.pane_padding
					bottomPadding: index % 2 ? Constants.groupbox_spacing : 0

					textStyle: index % 2 ? Style.text.hint_inverse : Style.text.hint_secondary_inverse
					text: display
				}
				ScrollBar.vertical: ScrollBar {
					id: verticalScrollbar

					width: ApplicationModel.scaleFactor * 18
					anchors.horizontalCenter: parent.right
					anchors.horizontalCenterOffset: Constants.pane_padding / 2

					policy: ScrollBar.AlwaysOn
					minimumSize: Style.dimens.minimumScrollBarSize
					stepSize: logView.height / logView.contentHeight
				}

				Connections {
					target: LogModel
					onFireNewLogMsg: if (logView.atYEnd) logView.positionViewAtEnd()
				}
			}

			FocusFrame {
				scope: logView
				framee: logView
				dynamic: false
				border.color: Constants.black
			}
		}
	}

	Keys.onPressed: {
		if (event.key === Qt.Key_PageDown)
			verticalScrollbar.increase()
		else if (event.key === Qt.Key_PageUp)
			verticalScrollbar.decrease()
		else if (event.key === Qt.Key_End)
			logView.positionViewAtEnd()
		else if (event.key === Qt.Key_Home)
			logView.positionViewAtBeginning()
	}

	ConfirmationPopup {
		id: confirmationPopup

		property bool deleteAll: true

		//: LABEL DESKTOP_QML
		title: (deleteAll ? qsTr("Delete old logfiles") : qsTr("Delete selected logfile")) + SettingsModel.translationTrigger
		//: INFO DESKTOP_QML The current/all logfile(s) are about to be removed, user confirmation required.
		text: (deleteAll ? qsTr("Please confirm that you want to delete your old logfiles.")
						 : qsTr("Please confirm that you want to delete the logfile.")
			   ) + SettingsModel.translationTrigger
		onConfirmed: deleteAll ? LogModel.removeOtherLogfiles() : LogModel.removeCurrentLogfile()
	}
}
