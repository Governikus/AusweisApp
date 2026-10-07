/**
 * Copyright (c) 2018-2026 Governikus Service GmbH, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

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

		onFilterClicked: {
			root.push(logFilterView);
		}
		onShareClicked: pPopupPosition => {
			logModel.shareLogFile(pPopupPosition);
		}
	}

	Component {
		id: logFilterView

		LogFilterView {
			enableTileStyle: root.enableTileStyle
			filterModel: logFilterModel
		}
	}
	LogFilterModel {
		id: logFilterModel

		sourceModel: LogModel {
			id: logModel
		}
	}
	GListView {
		id: logView

		anchors.fill: parent
		clip: true
		model: logFilterModel

		delegate: LogViewDelegate {
			boldFont: ListView.isCurrentItem && logView.activeFocus
			width: logView.width

			Accessible.onScrollDownAction: logView.scrollPageDown()
			Accessible.onScrollUpAction: logView.scrollPageUp()
			onActiveFocusChanged: if (activeFocus) {
				logView.centerViewAtIndex(index);
			}
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
