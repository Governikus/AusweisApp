/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
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

	signal keyPressed(int key)

	//: LABEL DESKTOP
	title: qsTr("Logs")

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Back

		onNavigationActionClicked: root.leaveView()
	}

	Keys.onPressed: event => {
		keyPressed(event.key);
	}

	TabbedPane {
		id: tabbedPane

		anchors.fill: parent
		contentDelegate: logContentDelegate
		contentRightMargin: 0
		sectionsModel: LogModel.logFileNames

		footerItem: ColumnLayout {
			spacing: Style.dimens.groupbox_spacing

			GButton {
				id: saveLog

				Layout.maximumWidth: Number.POSITIVE_INFINITY
				enabled: tabbedPane.sectionsModel.length > 0
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
			GButton {
				Layout.maximumWidth: Number.POSITIVE_INFINITY
				disabledTooltipText: qsTr("The current log will be automatically deleted at exit.")
				enableButton: tabbedPane.sectionsModel.length > 1
				icon.source: "qrc:///images/trash_icon.svg"
				//: LABEL DESKTOP
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

		onCurrentIndexChanged: LogModel.setLogFile(currentIndex)
	}
	Component {
		id: logContentDelegate

		GListView {
			id: logView

			activeFocusOnTab: true
			displayMarginBeginning: Style.dimens.pane_padding
			displayMarginEnd: Style.dimens.pane_padding
			implicitHeight: tabbedPane.availableHeight
			model: LogModel

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
					color: Style.color.pane.background.basic
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

				target: LogModel
			}
			Connections {
				function onKeyPressed(pKey) {
					logView.handleKeyPress(pKey);
				}

				target: root
			}
		}
	}
	ConfirmationPopup {
		id: confirmationPopup

		//: LABEL DESKTOP
		okButtonText: qsTr("Delete")
		//: INFO DESKTOP All logfiles are about to be removed, user confirmation required.
		text: qsTr("All old logs will be deleted.")
		//: LABEL DESKTOP
		title: qsTr("Delete all logs")
		width: UiPluginModel.scaleFactor * 360

		onConfirmed: LogModel.removeOtherLogFiles()
	}
}
