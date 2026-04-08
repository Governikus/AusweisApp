/**
 * Copyright (c) 2020-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.Type
import Governikus.View

GListView {
	id: root

	property real maximumContentWidth: Number.POSITIVE_INFINITY

	function isListElementEmptyFunc(pItem) {
		let delegate = pItem as FormattedTextPaneDelegate;
		if (delegate)
			return delegate.content === "";
		return true;
	}

	displayMarginBeginning: Style.dimens.pane_padding
	displayMarginEnd: Style.dimens.pane_padding
	footerPositioning: ListView.InlineFooter
	model: releaseInformationModel.currentRelease

	delegate: ReleaseNotesDelegate {
		anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
		anchors.horizontalCenterOffset: -Style.dimens.pane_padding / 2
		count: root.count
		width: Math.min(root.width - Style.dimens.pane_padding, root.maximumContentWidth)

		onActiveFocusChanged: if (activeFocus) {
			root.handleItemFocused(index);
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
	highlight: Item {
		z: 2

		FocusFrame {
			anchors.leftMargin: 0
			anchors.rightMargin: 0
			scope: root
			visible: Style.is_layout_desktop
		}
	}

	Component.onCompleted: releaseInformationModel.update()

	layer {
		enabled: Style.is_layout_desktop

		effect: GDropShadow {
		}
	}
	ReleaseInformationModel {
		id: releaseInformationModel

	}
}
