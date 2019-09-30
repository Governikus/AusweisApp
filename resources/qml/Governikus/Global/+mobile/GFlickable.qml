/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.10

import Governikus.Style 1.0

Flickable {
	id: baseItem

	property bool scrollBarEnabled: true
	property real scrollBarTopPadding: 0
	property real scrollBarBottomPadding: 0

	function scrollPageDown() {
		if (contentHeight > height) {
			Utils.scrollPageDown(baseItem)
		}
	}

	function scrollPageUp() {
		if (contentHeight > height) {
			Utils.scrollPageUp(baseItem)
		}
	}

	function highlightScrollbar() {
		if (ScrollBar.vertical) ScrollBar.vertical.highlight()
	}

	ScrollBar.vertical: scrollBarEnabled ? scrollBar.createObject() : null

	maximumFlickVelocity: Constants.scrolling_speed
	flickableDirection: Flickable.VerticalFlick

	boundsMovement: Flickable.FollowBoundsBehavior
	boundsBehavior: contentHeight <= height ? Flickable.StopAtBounds : Flickable.DragAndOvershootBounds

	onVisibleChanged: if (visible) highlightScrollbar()

	Component {
		id: scrollBar
		GScrollBar {
			topPadding: baseItem.scrollBarTopPadding + Style.dimens.scrollbar_padding
			bottomPadding: baseItem.scrollBarBottomPadding + Style.dimens.scrollbar_padding
		}
	}
}
