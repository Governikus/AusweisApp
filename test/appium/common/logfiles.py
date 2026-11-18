import os
import pathlib
import shutil


def find_logfiles():
    log_dir = os.getenv('AUSWEISAPP_LOGFILE_FOLDER')
    if not log_dir:
        return (
            [],
            'AUSWEISAPP_LOGFILE_FOLDER is not defined, cannot retrieve logfiles',
        )

    logfiles = list(pathlib.Path(log_dir).glob('AusweisApp.*.log'))
    if len(logfiles) == 1:
        return logfiles, None

    return (
        [],
        f'Expected exactly one logfile in {log_dir} but found {logfiles}',
    )


def copy_logfile(log_name):
    save_folder = os.getenv('APPIUM_LOGFILE_SAVE_FOLDER')
    if not save_folder:
        return 'APPIUM_LOGFILE_SAVE_FOLDER is not defined'

    logfiles, error_string = find_logfiles()
    if error_string:
        return error_string

    pathlib.Path(save_folder).mkdir(parents=True, exist_ok=True)

    shutil.copy(
        logfiles[0],
        os.path.join(save_folder, log_name),
    )
    return None
