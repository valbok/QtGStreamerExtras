TEMPLATE = subdirs

SUBDIRS += gstreamerextras

qtHaveModule(quick):qtConfig(opengl) {
    src_imports.subdir = imports
    src_imports.depends = gstreamerextras

    SUBDIRS += \
        src_imports
}
