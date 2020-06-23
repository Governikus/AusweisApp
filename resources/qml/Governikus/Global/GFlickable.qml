/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

import Governikus.Global 1.0
import Governikus.Style 1.0

Flickable {
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

	function positionViewAtBeginning() {
		contentY = originY
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
			baseItem.contentY = baseItem.contentHeight - baseItem.height
		else if (key === Qt.Key_Home)
			baseItem.contentY = baseItem.originY
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
