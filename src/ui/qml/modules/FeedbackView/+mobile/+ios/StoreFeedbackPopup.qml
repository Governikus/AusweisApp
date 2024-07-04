/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Type

Item {
	function open() {
		ApplicationModel.showAppStoreRatingDialog();
	}
}
