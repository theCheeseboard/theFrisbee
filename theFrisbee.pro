TEMPLATE = subdirs

libproj.subdir = libthefrisbee

appproj.subdir = application
appproj.depends = libproj

SUBDIRS = libproj appproj
