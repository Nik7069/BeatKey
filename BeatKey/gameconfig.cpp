#include "gameconfig.h"
#include <QDebug>

GameConfig::GameConfig()
    : m_settings(QCoreApplication::applicationDirPath() + "/settings.ini",
                 QSettings::IniFormat)
{
    loadSettings();
}

void GameConfig::loadSettings()
{
    m_hitVolume = m_settings.value("Sound/hitVolume", 0.4f).toFloat();
    m_missVolume = m_settings.value("Sound/missVolume", 0.4f).toFloat();
    m_musicVolume = m_settings.value("Sound/musicVolume", 0.5f).toFloat();
    m_primaryKey = m_settings.value("Keys/primaryKey", Qt::Key_Space).toInt();
    m_secondaryKey = m_settings.value("Keys/secondaryKey", Qt::Key_X).toInt();

    qDebug() << "Settings loaded:"
             << "hitVol:" << m_hitVolume
             << "missVol:" << m_missVolume
             << "musicVol:" << m_musicVolume
             << "primaryKey:" << m_primaryKey
             << "secondaryKey:" << m_secondaryKey;
}

void GameConfig::saveSettings()
{
    m_settings.setValue("Sound/hitVolume", m_hitVolume);
    m_settings.setValue("Sound/missVolume", m_missVolume);
    m_settings.setValue("Sound/musicVolume", m_musicVolume);
    m_settings.setValue("Keys/primaryKey", m_primaryKey);
    m_settings.setValue("Keys/secondaryKey", m_secondaryKey);

    m_settings.sync();
    qDebug() << "Settings saved";
}

void GameConfig::setHitVolume(float volume)
{
    m_hitVolume = qBound(0.0f, volume, 1.0f);
}

void GameConfig::setMissVolume(float volume)
{
    m_missVolume = qBound(0.0f, volume, 1.0f);
}

void GameConfig::setMusicVolume(float volume)
{
    m_musicVolume = qBound(0.0f, volume, 1.0f);
}

void GameConfig::setPrimaryKey(int key)
{
    m_primaryKey = key;
}

void GameConfig::setSecondaryKey(int key)
{
    m_secondaryKey = key;
}
