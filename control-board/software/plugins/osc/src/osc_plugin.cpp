
#include "osc.h"
#include "osc_plugin.h"

#include <QtPlugin>

OscPlugin::OscPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void OscPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;
    m_initialized = true;
 }

bool OscPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *OscPlugin::createWidget(QWidget *parent)
{
    return new Osc(parent);
}

QString OscPlugin::name() const
{
    return "Osc";
}

QString OscPlugin::group() const
{
    return "Display Widgets [Examples]";
}

QIcon OscPlugin::icon() const
{
    return QIcon();
}

QString OscPlugin::toolTip() const
{
    return "";
}

QString OscPlugin::whatsThis() const
{
    return "";
}

bool OscPlugin::isContainer() const
{
    return false;
}

QString OscPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
           " <widget class=\"Osc\" name=\"osc\">\n"
           "  <property name=\"geometry\">\n"
           "   <rect>\n"
           "    <x>0</x>\n"
           "    <y>0</y>\n"
           "    <width>256</width>\n"
           "    <height>256</height>\n"
           "   </rect>\n"
           "  </property>\n"
           "  <property name=\"toolTip\" >\n"
           "   <string>Oscilloscope</string>\n"
           "  </property>\n"
           "  <property name=\"whatsThis\" >\n"
           "   <string>Plot for siaplaying curves.</string>\n"
           "  </property>\n"
           " </widget>\n"
           "</ui>\n";
}

QString OscPlugin::includeFile() const
{
    return "osc.h";
}

Q_EXPORT_PLUGIN2( osc, OscPlugin )



