/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
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

	property string logFileName: "not set"
	property alias logFilePath: logModel.source

	contentIsScrolled: !logView.atYBeginning
	title: root.logFileName

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.pop()
	}
	rightTitleBarAction: LogTitleBarControls {
		showFilter: true
		showShare: true

		onFilterChanged: logStack.currentIndex = filter ? 0 : 1
		onShareClicked: pPopupPosition => {
			logModel.shareLogFile(pPopupPosition);
		}
	}

	LogModel {
		id: logModel

	}
	LogFilterModel {
		id: filterModel

		sourceModel: logModel
	}
	StackLayout {
		id: logStack

		anchors.fill: parent
		currentIndex: 1

		GFlickableColumnLayout {
			clip: true
			spacing: Style.dimens.text_spacing

			GOptionsContainer {
				Layout.fillWidth: true
				containerPadding: Style.dimens.pane_padding
				containerSpacing: Style.dimens.groupbox_spacing
				//: MOBILE
				title: qsTr("Filter")

				Subheading {
					//: MOBILE
					text: qsTr("Level")
				}
				GridLayout {
					columnSpacing: Style.dimens.groupbox_spacing
					columns: Math.max(1, (parent.width + columnSpacing) / (levelRepeater.maxItemWidth + columnSpacing))
					rowSpacing: Style.dimens.groupbox_spacing
					uniformCellWidths: true

					GRepeater {
						id: levelRepeater

						model: filterModel.levels

						delegate: GCheckBox {
							required property string modelData

							checked: filterModel.selectedLevels.indexOf(text) !== -1
							text: modelData

							onCheckedChanged: filterModel.configureLevel(text, checked)
						}
					}
				}
				Subheading {
					//: MOBILE
					text: qsTr("Category")
				}
				GridLayout {
					columnSpacing: Style.dimens.groupbox_spacing
					columns: Math.max(1, (parent.width + columnSpacing) / (categoryRepeater.maxItemWidth + columnSpacing))
					rowSpacing: Style.dimens.groupbox_spacing
					uniformCellWidths: true

					GRepeater {
						id: categoryRepeater

						model: filterModel.categories

						delegate: GCheckBox {
							required property string modelData

							checked: filterModel.selectedCategories.indexOf(text) !== -1
							text: modelData

							onCheckedChanged: filterModel.configureCategory(text, checked)
						}
					}
				}
			}
		}
		GListView {
			id: logView

			Layout.fillHeight: true
			Layout.fillWidth: true
			clip: true
			model: filterModel

			delegate: LogViewDelegate {
				boldFont: ListView.isCurrentItem && logView.activeFocus
				width: logView.width

				Accessible.onScrollDownAction: logView.scrollPageDown()
				Accessible.onScrollUpAction: logView.scrollPageUp()
			}

			Connections {
				function onFireNewLogMsg() {
					if (logView.atYEnd)
						logView.positionViewAtEnd();
				}

				target: logModel
			}
			GText {
				anchors.centerIn: parent
				horizontalAlignment: Text.AlignHCenter
				//: MOBILE No log entries, placeholder text.
				text: qsTr("Currently there are no log entries matching your filter.")
				visible: logView.count === 0
				width: parent.width - 2 * Style.dimens.pane_spacing
			}
		}
	}
}
