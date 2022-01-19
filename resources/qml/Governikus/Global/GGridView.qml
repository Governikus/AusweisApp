/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0

GridView {
	id: baseItem

	property bool scrollBarEnabled: true
	property real scrollBarTopPadding: 0
	property real scrollBarBottomPadding: 0

	function scrollPageDown() {
		Utils.scrollPageDown(baseItem)
	}

	function scrollPageUp() {
		Utils.scrollPageUp(baseItem)
	}

	function highlightScrollbar() {
		if (ScrollBar.vertical) ScrollBar.vertical.highlight()
	}

	ScrollBar.vertical: scrollBarEnabled ? scrollBar.createObject() : null

	function handleKeyPress(key) {
		if (key === Qt.Key_PageDown)
			baseItem.scrollPageDown()
		else if (key === Qt.Key_PageUp)
			baseItem.scrollPageUp()
		else if (key === Qt.Key_End)
			baseItem.positionViewAtEnd()
		else if (key === Qt.Key_Home)
			baseItem.positionViewAtBeginning()
	}

	Keys.onPressed: handleKeyPress(event.key)

	maximumFlickVelocity: Constants.scrolling_speed
	flickDeceleration: Constants.flickDeceleration
	flickableDirection: Flickable.VerticalFlick

	boundsMovement: Flickable.FollowBoundsBehavior
	boundsBehavior: Constants.is_desktop ? Flickable.StopAtBounds : (contentHeight <= height ? Flickable.StopAtBounds : Flickable.DragAndOvershootBounds)

	onVisibleChanged: if (visible) highlightScrollbar()

	Component {
		id: scrollBar
		GScrollBar {
			topPadding: baseItem.scrollBarTopPadding + Style.dimens.scrollbar_padding_vertical
			bottomPadding: baseItem.scrollBarBottomPadding + Style.dimens.scrollbar_padding_vertical
		}
	}
}
