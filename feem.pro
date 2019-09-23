TEMPLATE = subdirs

SUBDIRS += \
    ./qtribbon/src/shared/ribbon \
    ./qtribbon/src/ribbondsgn \
    ./qtribbon/ribbonsample/ribbonsample.pro \
    ./bounce/bounce.pro \
    ./feem/feem.pro \
    ./Optimize/Optimize.pro \

TRANSLATIONS = $$PWD/feem/res/translations/feem_en.ts \
                $$PWD/feem/res/translations/feem_zh.ts \

CONFIG += ordered
