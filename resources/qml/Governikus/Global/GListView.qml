/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style

ListView {
	id: baseItem

	property bool scrollBarAutohide: !Constants.is_desktop
	property real scrollBarBottomPadding: 0
	property color scrollBarColor: Style.color.control
	property bool scrollBarEnabled: true
	property real scrollBarTopPadding: 0

	function handleKeyPress(key) {
		if (key === Qt.Key_PageDown)
			baseItem.scrollPageDown();
		else if (key === Qt.Key_PageUp)
			baseItem.scrollPageUp();
		else if (key === Qt.Key_End)
			baseItem.positionViewAtEnd();
		else if (key === Qt.Key_Home)
			baseItem.positionViewAtBeginning();
	}
	function highlightScrollbar() {
		if (ScrollBar.vertical)
			ScrollBar.vertical.highlight();
	}
	function scrollPageDown() {
		Utils.scrollPageDown(baseItem);
	}
	function scrollPageUp() {
		Utils.scrollPageUp(baseItem);
	}

	Accessible.focusable: false
	Accessible.role: Accessible.ScrollBar
	ScrollBar.vertical: scrollBarEnabled ? scrollBar.createObject() : null
	boundsBehavior: Constants.is_desktop ? Flickable.StopAtBounds : (contentHeight <= height ? Flickable.StopAtBounds : Flickable.DragAndOvershootBounds)
	boundsMovement: Flickable.FollowBoundsBehavior
	flickDeceleration: Constants.flickDeceleration
	flickableDirection: Flickable.VerticalFlick
	maximumFlickVelocity: Constants.scrolling_speed

	Accessible.onDecreaseAction: scrollPageUp()
	Accessible.onIncreaseAction: scrollPageDown()
	Keys.onPressed: event => {
		handleKeyPress(event.key);
	}
	onVisibleChanged: if (visible)
		highlightScrollbar()

	Component {
		id: scrollBar

		GScrollBar {
			autohide: scrollBarAutohide
			bottomPadding: baseItem.scrollBarBottomPadding + Style.dimens.scrollbar_padding_vertical
			color: baseItem.scrollBarColor
			topPadding: baseItem.scrollBarTopPadding + Style.dimens.scrollbar_padding_vertical
		}
	}
}
