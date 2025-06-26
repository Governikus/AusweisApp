/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Type

Item {
	function close() {
	}
	function open() {
		ApplicationModel.showAppStoreRatingDialog();
	}
}
