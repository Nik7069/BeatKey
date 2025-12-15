#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <QSettings>
#include <QCoreApplication>

class GameConfig
{
public:
    static GameConfig& instance()
    {
        static GameConfig config;
        return config;
    }

    // Геттеры
    float hitVolume() const { return m_hitVolume; }
    float missVolume() const { return m_missVolume; }
    float musicVolume() const { return m_musicVolume; }
    int primaryKey() const { return m_primaryKey; }
    int secondaryKey() const { return m_secondaryKey; }

    // Сеттеры
    void setHitVolume(float volume);
    void setMissVolume(float volume);
    void setMusicVolume(float volume);
    void setPrimaryKey(int key);
    void setSecondaryKey(int key);

    // Загрузка и сохранение
    void loadSettings();
    void saveSettings();

private:
    GameConfig();

    float m_hitVolume = 0.4f;
    float m_missVolume = 0.4f;
    float m_musicVolume = 0.5f;
    int m_primaryKey = Qt::Key_Space;
    int m_secondaryKey = Qt::Key_X;

    QSettings m_settings;
};

#endif // GAMECONFIG_H
