/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0

Item {
	signal firePush(var pSectionPage)
	signal firePushWithProperties(var pSectionPage, var pProperties)
	signal fireReplace(var pSectionPage)
	signal firePop()
	signal firePopAll()

	function scrollPageDown() {
		sectionPageFlickable.scrollPageDown()
	}

	function scrollPageUp() {
		sectionPageFlickable.scrollPageUp()
	}

	function highlightScrollbar() {
		sectionPageFlickable.highlightScrollbar()
	}

	property var onActivated: highlightScrollbar

	property bool pushed: false

	property bool topLevelPage: false

	property var navigationAction: null
	property string title: ""
	property var rightTitleBarAction: null
	property var subTitleBarAction: null

	property bool titleBarVisible: true
	property color titleBarColor: Style.color.accent
	/* if the header component has a property named titleBarOpacity, use it, otherwise use default value*/
	readonly property real titleBarOpacity: header !== null && typeof(header.titleBarOpacity) != "undefined" ? header.titleBarOpacity : 1
	// If a header is set, it is shown as background of the TitleBar, so we need to expand the height
	property bool contentBehindTitlebar: header !== null && typeof(header.titleBarOpacity) != "undefined"

	// Main flickable of this view
	property var sectionPageFlickable: flickable

	// When enabled the section page will automatically add a safeAreaMargin to the bottom of the page
	property bool automaticSafeAreaMarginHandling: true

	// Default header/content of flickable
	property QtObject header
	property QtObject content

	Accessible.onScrollDownAction: flickable.scrollPageDown()
	Accessible.onScrollUpAction: flickable.scrollPageUp()

	GFlickable {
		id: flickable

		height: contentBehindTitlebar ? (parent.height + Style.dimens.titlebar_height) : parent.height
		width: parent.width
		anchors.bottom: parent.bottom

		scrollBarTopPadding: contentBehindTitlebar ? Style.dimens.titlebar_height : 0

		contentWidth: flickableContent.width
		contentHeight: flickableContent.height

		Column {
			id: flickableContent

			children: [header, content]
		}
	}
}
