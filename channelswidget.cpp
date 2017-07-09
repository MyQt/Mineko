#include "channelswidget.h"
#include "ui_channelswidget.h"
#include <QPropertyAnimation>
#include <QSettings>

ChannelsWidget::ChannelsWidget(QWidget *parent, QNetworkCookieJar *cookJar) :
    QFrame(parent),
    ui(new Ui::ChannelsWidget)
{
    ui->setupUi(this);
    setFrameStyle(QFrame::NoFrame);
    setPalette(parent->palette());
    m_getChannelList = new GetBaiduChannel(0, cookJar);
    m_gridlayOut = new QGridLayout;

    m_channelList = getChannelList();

    for(int i = 0, k = 0; i < m_channelList.size() / 4; i++, k += 4)
    {
        for(int j = 0; j < 4; j++)
        {
            QPushButton *btn = new QPushButton(this);
            btn->setAutoFillBackground(true);
            btn->setText(m_channelList.at(k + j).channelName);
            btn->setPalette(parent->palette());
            btn->setBackgroundRole(palette().Window);
            btn->setAutoFillBackground(true);
            m_gridlayOut->addWidget(btn, i, j);
            m_btnGroup.addButton(btn, k + j);
        }
    }

    setLayout(m_gridlayOut);

    connect( &m_btnGroup, SIGNAL(buttonClicked (int)), this, SLOT(channelSelected(int)) );
    m_iLastChannelIndex = -1;
    m_selectedList.clear();
}

void ChannelsWidget::channelSelected(int iChannel)
{
    if (m_iLastChannelIndex == iChannel)
    {
        return;
    }
    m_selectedList.push_back(iChannel);
    setButtonStyle(m_iLastChannelIndex, selected);
    setButtonStyle(iChannel, select_now);
    m_iLastChannelIndex = iChannel;
    emit channelChanged(m_channelList.at(iChannel));

    //记录当前频道信息
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Mineko", "Mineko");
    settings.beginGroup("User");
    settings.setValue("ChannelId", m_channelList.at(iChannel).channelId);
    settings.setValue("ChannelName", m_channelList.at(iChannel).channelName);
    settings.setValue("ChannelIndex", iChannel);
    settings.endGroup();
}


ChannelsWidget::~ChannelsWidget()
{
    delete ui;
}

void ChannelsWidget::leaveEvent(QEvent *event)
{
    hide();
}

QList<CHANNEL_INFO> ChannelsWidget::getChannelList()
{
    return m_getChannelList->getMusicChannel();
}

void ChannelsWidget::bgColorChanged(QColor color)
{
    m_color = color;
    for (int i = 0; i < m_btnGroup.buttons().size(); i++)
    {
        selectType type = select_none;
        QList<int>::iterator iter = m_selectedList.begin();
        for(;iter != m_selectedList.end(); iter++)
        {
            if ((*iter) == i)
            {
                type = selected;
                if (i == m_iLastChannelIndex)
                {
                    type = select_now;
                }
                break;
            }
        }
        setButtonStyle(i, type);
    }
}

void ChannelsWidget::setButtonStyle(int index, selectType type)
{
    QPushButton* pBtn = (QPushButton* )m_btnGroup.button(index);
    if (pBtn)
    {
        QColor arcColor = QColor(255-m_color.red(), 255-m_color.green(), 255-m_color.blue(),m_color.alpha());
        QString styleSheet;
        styleSheet.append("QPushButton{border-style: none;");
        styleSheet.append("background-color:").append(m_color.name()).append(";");
        if (type == select_now)
        {
            styleSheet.append("font-weight:600;");
            styleSheet.append("color:").append(m_color.darker(300).name()).append(";}");
        } else if (type == selected)
        {
            styleSheet.append("color:").append(m_color.lighter(50).name()).append(";}");

        } else if (type == select_none)
        {
            styleSheet.append("color:").append(arcColor.name()).append(";}");
        }
        pBtn->setStyleSheet(styleSheet);
    }
}
