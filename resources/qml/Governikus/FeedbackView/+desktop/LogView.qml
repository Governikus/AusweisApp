/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type.LogModel

SectionPage {
	id: root

	signal keyPressed(int key)

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP
		text: qsTr("Logs")
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

		footerItem: Item {
			height: buttonLayout.implicitHeight

			ColumnLayout {
				id: buttonLayout

				spacing: Constants.groupbox_spacing

				anchors {
					left: parent.left
					right: parent.right
					rightMargin: Constants.groupbox_spacing
				}
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

					onClicked: ApplicationWindow.window.showDetachedLogView()
				}
			}
		}

		onCurrentIndexChanged: LogModel.setLogFile(currentIndex)
	}
	Component {
		id: logContentDelegate

		GListView {
			id: logView

			activeFocusOnTab: true
			displayMarginBeginning: Constants.pane_padding
			displayMarginEnd: Constants.pane_padding
			implicitHeight: tabbedPane.availableHeight
			model: LogModel

			delegate: FocusScope {
				readonly property bool isFirstItem: index === 0
				readonly property bool isLastItem: index === ListView.view.count - 1

				implicitHeight: logDelegate.implicitHeight + logDelegate.anchors.topMargin + logDelegate.anchors.bottomMargin
				width: logView.width - Constants.pane_padding

				RoundedRectangle {
					anchors.fill: parent
					bottomLeftCorner: isLastItem
					bottomRightCorner: isLastItem
					color: Style.color.pane
					topLeftCorner: isFirstItem
					topRightCorner: isFirstItem
				}
				LogViewDelegate {
					id: logDelegate

					focus: true

					anchors {
						bottomMargin: isLastItem ? Constants.pane_padding : 0
						fill: parent
						leftMargin: Constants.pane_padding
						rightMargin: Constants.pane_padding
						topMargin: isFirstItem ? Constants.pane_padding : Constants.text_spacing
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
		width: plugin.scaleFactor * 600

		onConfirmed: LogModel.removeOtherLogFiles()
	}
}
