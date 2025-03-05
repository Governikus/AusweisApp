/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type

SectionPage {
	id: root

	//: LABEL ANDROID IOS
	title: qsTr("Log")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.pop()
	}
	rightTitleBarAction: LogTitleBarControls {
		allowRemoveAll: comboBox.model.length > 1 // comboBox.count doesn't seem to update reliably

		onRemoveAll: {
			confirmationPopup.open();
		}
		onShare: pPopupPosition => {
			LogModel.shareLog(pPopupPosition);
		}
	}

	Connections {
		function onActivate() {
			logView.highlightScrollbar();
		}
	}
	LogFilterModel {
		id: filterModel

	}
	ColumnLayout {
		anchors.fill: parent
		spacing: 0

		Rectangle {
			id: logSelector

			Layout.fillWidth: true
			Layout.preferredHeight: comboBox.height + Style.dimens.pane_padding * 2
			color: Style.color.paneSublevel.background.basic

			GComboBox {
				id: comboBox

				Accessible.description: qsTr("Select log from list.")
				model: LogModel.logFileNames

				onCurrentIndexChanged: LogModel.setLogFile(currentIndex)

				anchors {
					left: parent.left
					leftMargin: Style.dimens.pane_padding
					right: filterButton.left
					rightMargin: Style.dimens.pane_spacing
					top: parent.top
					topMargin: Style.dimens.pane_padding
				}
			}
			TitleBarAction {
				id: filterButton

				property bool filter: false

				Accessible.checked: filter
				Accessible.name: qsTr("Filter")
				Accessible.role: Accessible.CheckBox
				icon.source: filter ? "qrc:///images/filter_off.svg" : "qrc:///images/filter.svg"
				iconTintColor: comboBox.textStyle.textColor

				onClicked: filter = !filter

				anchors {
					right: parent.right
					rightMargin: Style.dimens.pane_padding
					verticalCenter: comboBox.verticalCenter
				}
			}
		}
		GFlickableColumnLayout {
			Layout.fillHeight: true
			Layout.fillWidth: true
			clip: true
			spacing: Style.dimens.text_spacing
			visible: filterButton.filter

			GOptionsContainer {
				Layout.fillWidth: true
				containerPadding: Style.dimens.pane_padding
				containerSpacing: Style.dimens.groupbox_spacing
				//: LABEL ANDROID IOS
				title: qsTr("Filter")

				GText {
					//: LABEL ANDROID IOS
					text: qsTr("Level")
					textStyle: Style.text.subline
				}
				Grid {
					columnSpacing: Style.dimens.groupbox_spacing
					columns: (width + columnSpacing) / (levelRepeater.maxItemWidth + columnSpacing)
					rowSpacing: Style.dimens.groupbox_spacing
					width: parent.width

					GRepeater {
						id: levelRepeater

						model: filterModel.levels

						delegate: GCheckBox {
							required property string modelData

							checked: filterModel.selectedLevels.indexOf(text) !== -1
							text: modelData
							width: levelRepeater.maxItemWidth

							onCheckedChanged: filterModel.configureLevel(text, checked)
						}
					}
				}
				GText {
					//: LABEL ANDROID IOS
					text: qsTr("Category")
					textStyle: Style.text.subline
				}
				Grid {
					columnSpacing: Style.dimens.groupbox_spacing
					columns: (width + columnSpacing) / (categoryRepeater.maxItemWidth + columnSpacing)
					rowSpacing: Style.dimens.groupbox_spacing
					width: parent.width

					GRepeater {
						id: categoryRepeater

						model: filterModel.categories

						delegate: GCheckBox {
							required property string modelData

							checked: filterModel.selectedCategories.indexOf(text) !== -1
							text: modelData
							width: categoryRepeater.maxItemWidth

							onCheckedChanged: filterModel.configureCategory(text, checked)
						}
					}
				}
			}
		}
		GListView {
			id: logView

			Accessible.ignored: false
			Layout.fillHeight: true
			Layout.fillWidth: true
			activeFocusOnTab: true
			clip: true
			model: filterModel
			visible: !filterButton.filter

			delegate: ListItem {
				required property int index
				readonly property bool isLastItem: index === ListView.view.count - 1
				required property string message
				required property string modelData
				required property string origin

				boldFont: ListView.isCurrentItem && logView.focus
				headerText: origin
				showSeparator: !isLastItem
				text: message

				Accessible.onScrollDownAction: (ListView.view as GListView).scrollPageDown()
				Accessible.onScrollUpAction: (ListView.view as GListView).scrollPageUp()
				onPressAndHold: {
					ApplicationModel.setClipboardText(modelData);
					//: INFO ANDROID IOS Toast message used to confirm the copy of a log entry.
					ApplicationModel.showFeedback(qsTr("The log entry was copied to the clipboard."));
				}
			}

			Connections {
				function onFireNewLogMsg() {
					if (logView.atYEnd)
						logView.positionViewAtEnd();
				}

				target: LogModel
			}
			GText {
				anchors.centerIn: parent
				horizontalAlignment: Text.AlignHCenter
				//: INFO ANDROID IOS No log entries, placeholder text.
				text: qsTr("Currently there are no log entries matching your filter.")
				visible: logView.count === 0
				width: parent.width - 2 * Style.dimens.pane_spacing
			}
		}
	}
	ConfirmationPopup {
		id: confirmationPopup

		//: LABEL ANDROID IOS
		okButtonText: qsTr("Delete")
		//: INFO ANDROID IOS All logfiles are about to be removed, user confirmation required.
		text: qsTr("All old logs will be deleted.")
		//: LABEL ANDROID IOS
		title: qsTr("Delete all logs")

		onConfirmed: LogModel.removeOtherLogFiles()
	}
}
