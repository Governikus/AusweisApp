/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import Governikus.Global

AbstractButton {
	Accessible.onScrollDownAction: Utils.scrollPageDownOnGFlickable(this)
	Accessible.onScrollUpAction: Utils.scrollPageUpOnGFlickable(this)
	Keys.onEnterPressed: clicked()
	Keys.onReturnPressed: clicked()
	onFocusChanged: if (focus)
		Utils.positionViewAtItem(this)
}
