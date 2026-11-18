/**
 * Copyright (c) 2020-2026 Governikus Service GmbH, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.Type

GListView {
	id: root

	property real maximumContentWidth: Number.POSITIVE_INFINITY

	displayMarginBeginning: Style.dimens.pane_padding
	displayMarginEnd: Style.dimens.pane_padding
	enableLineShineThroughFix: true
	footerPositioning: ListView.InlineFooter
	keyNavigationChangesCurrentIndex: false
	model: releaseInformationModel.currentRelease

	delegate: ReleaseNotesDelegate {
		anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
		anchors.horizontalCenterOffset: -Style.dimens.pane_padding / 2
		count: root.count
		enabled: content !== ''
		width: Math.min(root.width - Style.dimens.pane_padding, root.maximumContentWidth)

		onActiveFocusChanged: if (activeFocus) {
			root.centerViewAtIndex(index);
		}
		onScrollDownAction: root.scrollPageDown()
		onScrollUpAction: root.scrollPageUp()
	}
	footer: ColumnLayout {
		width: parent.width

		GButton {
			Layout.alignment: Style.scanPatternAlignment
			Layout.leftMargin: Style.is_layout_desktop ? 0 : -root.anchors.leftMargin
			Layout.topMargin: Style.dimens.pane_spacing
			icon.source: "qrc:///images/material_refresh.svg"
			//: ALL_PLATFORMS
			text: qsTr("Retry")
			tintIcon: true
			visible: releaseInformationModel.allowRetry

			onClicked: releaseInformationModel.update()
		}
	}

	Component.onCompleted: releaseInformationModel.update()

	layer {
		enabled: Style.is_layout_desktop && GraphicsInfo.api !== GraphicsInfo.Software

		effect: GDropShadow {
		}
	}
	ReleaseInformationModel {
		id: releaseInformationModel
	}
}
