/**
 * Copyright (c) 2025-2026 Governikus Service GmbH, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import Governikus.Global

AbstractButton {
	Accessible.onScrollDownAction: Utils.scrollPageDownOnGFlickable(this)
	Accessible.onScrollUpAction: Utils.scrollPageUpOnGFlickable(this)
	Accessible.onShowOnScreenAction: Utils.positionViewAtItem(this)
	Keys.onEnterPressed: clicked()
	Keys.onReturnPressed: clicked()
	onActiveFocusChanged: if (activeFocus)
		Utils.positionViewAtItem(this)
}
