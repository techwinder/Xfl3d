/****************************************************************************

    Xfl3d application
    Copyright (C) Andre Deperrois 
    License: GPL v3

*****************************************************************************/


#pragma once

#include <QColor>

#include <xflcore/fontstruct.h>


namespace DisplayOptions
{    
    // Potential issues when declaring fonts as variables when linking statically to Qt
    // From the Qt dochttps://doc.qt.io/qt-5/qfont.html:
    //    "Note that a QGuiApplication instance must exist before a QFont can be used."
    //
    extern FontStruct g_TextFontStruct;
    extern FontStruct g_TableFontStruct;
    extern FontStruct g_TreeFontStruct;
    extern FontStruct g_ToolTipFontStruct;

    extern QColor g_BackgroundColor2d;
    extern QColor g_TextColor;

    extern double g_ScaleFactor;
    extern double g_IconSize;

    enum enumThemeType {LIGHTTHEME, DARKTHEME, CUSTOMTHEME};
    extern  enumThemeType g_Theme;

    extern bool g_bStyleSheet;

    inline void setStyleSheetOverride(bool bSheet) {g_bStyleSheet=bSheet;}
    inline bool bStyleSheetOverride() {return g_bStyleSheet;}


    inline QColor backgroundColor() {return g_BackgroundColor2d;}
    inline void setBackgroundColor(QColor clr) {g_BackgroundColor2d=clr;}

    inline QColor const &textColor() {return g_TextColor;}
    inline void setTextColor(QColor clr) {g_TextColor=clr;}


    inline FontStruct const &textFontStruct()    {return g_TextFontStruct;}
    inline FontStruct const &tableFontStruct()   {return g_TableFontStruct;}
    inline FontStruct const &treeFontStruct()    {return g_TreeFontStruct;}
    inline FontStruct const &toolTipFontStruct() {return g_ToolTipFontStruct;}

    inline QFont textFont()    {return g_TextFontStruct.font();}
    inline QFont tableFont()   {return g_TableFontStruct.font();}
    inline QFont treeFont()    {return g_TreeFontStruct.font();}
    inline QFont toolTipFont() {return g_ToolTipFontStruct.font();}


    inline void setTheme(DisplayOptions::enumThemeType themetype) {g_Theme=themetype;}
    inline enumThemeType theme() {return g_Theme;}
    inline bool isLightTheme()   {return g_Theme==DisplayOptions::LIGHTTHEME;}
    inline bool isDarkTheme()    {return g_Theme==DisplayOptions::DARKTHEME;}

    bool isDarkMode();

    inline void setScaleFactor(double f) {g_ScaleFactor=f;}
    inline double scaleFactor() {return g_ScaleFactor;}

    inline void setIconSize(int s) {g_IconSize=s;}
    inline int iconSize() {return g_IconSize;}

    void setTextFont(   QFont const & fnt);
    void setTableFont(  QFont const & fnt);
    void setTreeFont(   QFont const & fnt);
    void setToolTipFont(QFont const & fnt);

    void listFontStruct(FontStruct const &fntstruct);

    void loadSettings(QSettings &settings);
    void saveSettings(QSettings &settings);

}

