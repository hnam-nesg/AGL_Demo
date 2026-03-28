TEMPLATE = aux

icon.path = /usr/share/icons/hicolor/scalable
icon.files += $$_PRO_FILE_PWD_/dashboard.svg
icon.CONFIG = no_check_exist

desktop.path = /usr/share/applications
desktop.files = $$_PRO_FILE_PWD_/dashboard.desktop
desktop.CONFIG = no_check_exist

INSTALLS += desktop icon
