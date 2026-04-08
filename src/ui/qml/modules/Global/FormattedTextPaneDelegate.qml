/**
 * Copyright (c) 2020-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Global
import Governikus.Style

GPaneBackgroundDelegate {
	id: root

	readonly property int bottomMargin: root.isLast ? Style.dimens.pane_padding : 0
	required property string content
	required property int index
	readonly property int topMargin: root.isFirst ? Style.dimens.pane_padding : 0
	required property int type

	signal scrollDownAction
	signal scrollUpAction

	Accessible.focusable: true
	Accessible.ignored: root.content === ""
	Accessible.name: delegate.Accessible.name
	Accessible.role: delegate.Accessible.role
	idx: index
	implicitHeight: delegate.implicitHeight + bottomMargin + topMargin

	Accessible.onScrollDownAction: root.scrollDownAction()
	Accessible.onScrollUpAction: root.scrollUpAction()

	FormattedTextDelegate {
		id: delegate

		Accessible.ignored: true
		content: root.content
		index: root.index
		type: root.type

		anchors {
			bottomMargin: root.bottomMargin
			fill: parent
			leftMargin: Style.dimens.pane_padding
			rightMargin: Style.dimens.pane_padding
			topMargin: root.topMargin
		}
	}
}
