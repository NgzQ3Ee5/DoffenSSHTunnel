PRECOMPILED_HEADER = pch.h

SOURCES += main.cpp\
    Paths.cpp \
    identifier.cpp \
    pch.h.cpp \
    ATLog.cpp \
    ATNamedAction.cpp \
    Backup.cpp \
    ComboBox.cpp \
    CustomAction.cpp \
    Images.cpp \
    TableView.cpp \
    TableWidget.cpp \
    TreeWidget.cpp \
    Widget.cpp \
    TunnelTreeWidget.cpp \
    PlainTextEdit.cpp \
    CustomActionsEditWidget.cpp \
    CustomButtonDialog.cpp \
    MoreTunnelsEditWidget.cpp \
    ATSkeleton.cpp \
    ATMainWindow.cpp \
    ChangePasswordDialog.cpp \
    CreatePasswordDialog.cpp \
    ExportHostsDialog.cpp \
    ImportHostsDialog.cpp \
    PasswordDialog.cpp \
    PreferencesDialog.cpp \
    RestoreDialog.cpp \
    SSHHostDialog.cpp \
    VariableSettingsDialog.cpp \
    PasswordDb.cpp \
    ctkSearchBox.cpp \
    treesearchwidget.cpp \
    treesearchwidgetlineedit.cpp \
    searchwidget.cpp \
    searchwidgetlineedit.cpp \
    OutputWindow.cpp

HEADERS  += \
    Paths.h \
    identifier.h \
    exceptions.h \
    pch.h \
    ATLog.h \
    ATNamedAction.h \
    Backup.h \
    ComboBox.h \
    CustomAction.h \
    Images.h \
    TableView.h \
    TableWidget.h \
    TreeWidget.h \
    Widget.h \
    TunnelTreeWidget.h \
    PlainTextEdit.h \
    ATSkeleton.h \
    CustomActionsEditWidget.h \
    CustomButtonDialog.h \
    MoreTunnelsEditWidget.h \
    ATMainWindow.h \
    ChangePasswordDialog.h \
    CreatePasswordDialog.h \
    ExportHostsDialog.h \
    ImportHostsDialog.h \
    PasswordDialog.h \
    PreferencesDialog.h \
    RestoreDialog.h \
    SSHHostDialog.h \
    VariableSettingsDialog.h \
    PasswordDb.h \
    ctkSearchBox.h \
    treesearchwidget.h \
    treesearchwidgetlineedit.h \
    searchwidget.h \
    searchwidgetlineedit.h \
    OutputWindow.h

FORMS    += \
    ATSkeleton.ui \
    ChangePasswordDialog.ui \
    CreatePasswordDialog.ui \
    CustomButtonDialog.ui \
    ExportHostsDialog.ui \
    ImportHostsDialog.ui \
    PasswordDialog.ui \
    PreferencesDialog.ui \
    RestoreDialog.ui \
    SSHHostDialog.ui \
    VariableSettingsDialog.ui \
    OutputWindow.ui

RESOURCES += \
    res/resource.qrc

OTHER_FILES += \
    res/uv_040_btn_down.png \
    res/app_010_connected.png \
    res/app_020_disconnected.png \
    res/ca_010_btn_add_action.png \
    res/ca_020_btn_edit.png \
    res/ca_030_btn_dup.png \
    res/ca_040_btn_del.png \
    res/ca_050_btn_left.png \
    res/ca_060_btn_up.png \
    res/ca_070_btn_down.png \
    res/ca_080_btn_right.png \
    res/ca_090_folder_open.png \
    res/ca_100_folder_closed.png \
    res/ca_110_action.png \
    res/cp_010_btn_view.png \
    res/ea_010_btn_add.png \
    res/ea_020_btn_del.png \
    res/ea_030_btn_up.png \
    res/ea_040_btn_down.png \
    res/ep_010_btn_view.png \
    res/ht_010_btn_add_folder.png \
    res/ht_020_btn_add_host.png \
    res/ht_030_btn_dup.png \
    res/ht_040_btn_del.png \
    res/ht_050_btn_connect.png \
    res/ht_060_btn_disconnect.png \
    res/ht_070_btn_clearlog.png \
    res/ht_080_btn_left.png \
    res/ht_090_btn_up.png \
    res/ht_100_btn_down.png \
    res/ht_110_btn_right.png \
    res/ht_120_host_disconnected.png \
    res/ht_130_host_connecting.png \
    res/ht_140_host_connected.png \
    res/ht_150_folder_open_disconnected.png \
    res/ht_160_folder_open_connected.png \
    res/ht_170_folder_closed_disconnected.png \
    res/ht_180_folder_closed_connected.png \
    res/ht_190_btn_search_up.png \
    res/ht_200_btn_search_down.png \
    res/ht_210_btn_search.png \
    res/ht_220_btn_search_clear.png \
    res/mt_010_btn_add.png \
    res/mt_020_btn_del.png \
    res/mt_030_btn_up.png \
    res/mt_040_btn_down.png \
    res/pw_010_btn_add.png \
    res/pw_020_btn_view_all.png \
    res/pw_030_btn_del.png \
    res/pw_040_btn_view.png \
    res/sd_010_btn_view.png \
    res/uv_010_btn_add.png \
    res/uv_020_btn_del.png \
    res/uv_030_btn_up.png

macx {
    CMD_SCRIPTS_1 = $$quote(chmod a+x $${PWD}/../../deps/macos_scripts/*.sh)
    message($$CMD_SCRIPTS_1)
    system($$CMD_SCRIPTS_1)

    #DoffenSSHTunnel.app/Contents/Info.plist
    QMAKE_INFO_PLIST = res/mac/Info.plist
    #Make icon:
    #cd ~/workspace/Qt/DoffenSSHTunnel/trunk/DoffenSSHTunnel/source/res/mac
    #sips -s format icns ~/workspace/Qt/DoffenSSHTunnel/trunk/deps/themes/icons/Hand\ Drawn\ 1/app_010_connected.png --out AppIcon.icns
    ICON = res/mac/AppIcon.icns
    #DoffenSSHTunnel.app/Contents/Resources/themes/icons/Hand_Drawn_1/
    APP_THEME_1.files = $$files($${PWD}/../../deps/themes/icons/Hand_Drawn_1/*.png)
    APP_THEME_1.path = Contents/Resources/themes/icons/Hand_Drawn_1
    QMAKE_BUNDLE_DATA += APP_THEME_1
    #DoffenSSHTunnel.app/Contents/Resources/iterm2win.sh
    APP_SCRIPTS_1.files = $$files($${PWD}/../../deps/macos_scripts/*.sh)
    APP_SCRIPTS_1.path = Contents/Resources
    QMAKE_BUNDLE_DATA += APP_SCRIPTS_1
    #DoffenSSHTunnel.app/Contents/MacOS/sshpass
    APP_SCRIPTS_2.files = $$files($${PWD}/../../deps/sshpass_v1.05_mymod/release_osx_mavericks/sshpass)
    APP_SCRIPTS_2.path = Contents/MacOS
    QMAKE_BUNDLE_DATA += APP_SCRIPTS_2
}
linux {
    #DoffenSSHTunnel/themes/icons/Hand_Drawn_1/
    CMD_THEMES = $$quote(cp -R $${PWD}/../../deps/themes $$shell_path($$OUT_PWD))
    message($$CMD_THEMES)
    system($$CMD_THEMES)
    #DoffenSSHTunnel shell scripts
    CMD_SCRIPTS_1 = $$quote(cp $${PWD}/../../deps/linux_scripts/*.sh $$shell_path($$OUT_PWD))
    message($$CMD_SCRIPTS_1)
    system($$CMD_SCRIPTS_1)
    CMD_SCRIPTS_2 = $$quote(chmod u+x $$shell_path($$OUT_PWD)/*.sh)
    message($$CMD_SCRIPTS_2)
    system($$CMD_SCRIPTS_2)
    #DoffenSSHTunnel/sshpass
    CMD_SSHPASS_1 = $$quote(cp $${PWD}/../../deps/sshpass_v1.05_mymod/release_ubuntu_1804_64bit/sshpass $$shell_path($$OUT_PWD))
    message($$CMD_SSHPASS_1)
    system($$CMD_SSHPASS_1)
    CMD_SSHPASS_2 = $$quote(chmod u+x $$shell_path($$OUT_PWD)/sshpass)
    message($$CMD_SSHPASS_2)
    system($$CMD_SSHPASS_2)

}
win* {
    #DoffenSSHTunnel/themes/icons/Hand_Drawn_1/
    DEBUG_OR_RELASE=debug
    CONFIG(release, debug|release) {
        DEBUG_OR_RELASE = release
    }
    CMD_THEMES = $$quote(robocopy /S $$shell_path($${PWD})\..\..\deps\themes $$shell_path($$OUT_PWD)\\$$DEBUG_OR_RELASE\themes *.*)
    message($$CMD_THEMES)
    system($$CMD_THEMES)
    #DoffenSSHTunnel/plink.exe & putty.exe
    CMD_PLINK = $$quote(robocopy /S $$shell_path($${PWD})\..\..\deps\putty_v0.83\release_win32 $$shell_path($$OUT_PWD)\\$$DEBUG_OR_RELASE *.*)
    message($$CMD_PLINK)
    system($$CMD_PLINK)
    #DoffenSSHTunnel/winscp.exe
    CMD_WINSCP = $$quote(robocopy /S $$shell_path($${PWD})\..\..\deps\winscp_v6.5\release_win32 $$shell_path($$OUT_PWD)\\$$DEBUG_OR_RELASE *.*)
    message($$CMD_WINSCP)
    system($$CMD_WINSCP)
    #DoffenSSHTunnel/<script files>
    CMD_SCRIPTS = $$quote(robocopy /S $$shell_path($${PWD})\..\..\deps\windows_scripts $$shell_path($$OUT_PWD)\\$$DEBUG_OR_RELASE *.*)
    message($$CMD_SCRIPTS)
    system($$CMD_SCRIPTS)
}

