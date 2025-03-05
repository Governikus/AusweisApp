/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Qt.labs.platform as Labs

Labs.FileDialog {
	function selectFile(pDefaultFilename) {
		currentFile = folder + "/" + pDefaultFilename;
		open();
	}

	fileMode: Labs.FileDialog.SaveFile
	folder: Labs.StandardPaths.writableLocation(Labs.StandardPaths.DocumentsLocation)

	Component.onDestruction: reject()
}
