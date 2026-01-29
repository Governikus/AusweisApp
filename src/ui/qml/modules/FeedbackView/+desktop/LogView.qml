/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Init
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type

SectionPage {
	id: root

	signal keyPressed(var pEvent)

	//: DESKTOP
	title: qsTr("Logs")

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Action.Back

		onNavigationActionClicked: root.leaveView()
	}

	Keys.onPressed: event => {
		keyPressed(event);
	}

	LogModel {
		id: logModel

	}
	TabbedPane {
		id: tabbedPane

		anchors.fill: parent
		contentDelegate: logContentDelegate
		contentRightMargin: 0
		sectionsModel: LogFilesModel

		footerItem: ColumnLayout {
			spacing: Style.dimens.groupbox_spacing

			GButton {
				id: saveLog

				Layout.maximumWidth: Number.POSITIVE_INFINITY
				enabled: LogFilesModel.count > 0
				icon.source: "qrc:///images/desktop/save_icon.svg"
				//: DESKTOP
				text: qsTr("Save log")
				tintIcon: true

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
			GButton {
				Layout.maximumWidth: Number.POSITIVE_INFINITY
				disabledTooltipText: qsTr("The current log will be automatically deleted at exit.")
				enableButton: LogFilesModel.count > 1
				icon.source: "qrc:///images/trash_icon.svg"
				//: DESKTOP
				text: qsTr("Delete all logs")
				tintIcon: true

				onClicked: confirmationPopup.open()
			}
			GButton {
				property QtObject detachedLogViewItem: null

				Layout.maximumWidth: Number.POSITIVE_INFINITY
				icon.source: "qrc:///images/desktop/material_open_in_browser.svg"
				text: qsTr("Detach log viewer")
				tintIcon: true

				onClicked: (ApplicationWindow.window as App).showDetachedLogView()
			}
		}

		onCurrentIndexChanged: logModel.source = LogFilesModel.getLogFilePath(currentIndex)
	}
	Component {
		id: logContentDelegate

		GListView {
			id: logView

			displayMarginBeginning: Style.dimens.pane_padding
			displayMarginEnd: Style.dimens.pane_padding
			implicitHeight: tabbedPane.availableHeight
			model: logModel

			delegate: FocusScope {
				id: logEntry

				required property int index
				readonly property bool isFirstItem: index === 0
				readonly property bool isLastItem: index === ListView.view.count - 1
				required property string level
				required property string message
				required property string origin

				implicitHeight: logDelegate.implicitHeight + logDelegate.anchors.topMargin + logDelegate.anchors.bottomMargin
				width: logView.width - Style.dimens.pane_padding

				onActiveFocusChanged: if (activeFocus) {
					logView.handleItemFocused(index);
				}

				RoundedRectangle {
					anchors.fill: parent
					bottomLeftCorner: logEntry.isLastItem
					bottomRightCorner: logEntry.isLastItem
					color: Style.color.pane.background.basic_unchecked
					topLeftCorner: logEntry.isFirstItem
					topRightCorner: logEntry.isFirstItem
				}
				LogViewDelegate {
					id: logDelegate

					focus: true
					level: logEntry.level
					message: logEntry.message
					origin: logEntry.origin

					anchors {
						bottomMargin: logEntry.isLastItem ? Style.dimens.pane_padding : 0
						fill: parent
						leftMargin: Style.dimens.pane_padding
						rightMargin: Style.dimens.pane_padding
						topMargin: logEntry.isFirstItem ? Style.dimens.pane_padding : Style.dimens.text_spacing
					}
				}
			}

			layer {
				enabled: GraphicsInfo.api !== GraphicsInfo.Software

				effect: GDropShadow {
				}
			}
			Connections {
				function onFireNewLogMsg() {
					if (logView.atYEnd)
						logView.positionViewAtEnd();
				}

				target: logModel
			}
			Connections {
				function onKeyPressed(pEvent) {
					logView.handleKeyPress(pEvent);
				}

				target: root
			}
		}
	}
	ConfirmationPopup {
		id: confirmationPopup

		//: DESKTOP
		okButtonText: qsTr("Delete")
		//: DESKTOP All logfiles are about to be removed, user confirmation required.
		text: qsTr("All old logs will be deleted.")
		//: DESKTOP
		title: qsTr("Delete all logs")
		width: UiPluginModel.scaleFactor * 360

		onConfirmed: LogFilesModel.removeOtherLogFiles()
	}
}
