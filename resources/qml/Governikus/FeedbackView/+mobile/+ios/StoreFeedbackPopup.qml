/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Type.ApplicationModel 1.0

Item {
	function open() {
		ApplicationModel.showAppStoreRatingDialog();
	}
}
