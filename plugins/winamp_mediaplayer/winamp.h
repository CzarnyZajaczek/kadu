/*
 * Copyright (C) 2009 Michal Podsiadlik <michal@kadu.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Clarified Artistic License (see LICENSE.txt
 * for details).
 */

#pragma once

#include "plugins/mediaplayer/player-commands.h"
#include "plugins/mediaplayer/player-info.h"

#include <QtCore/QObject>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class WinampMediaPlayer : public QObject, public PlayerCommands, public PlayerInfo
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit WinampMediaPlayer(QObject *parent = nullptr);
    virtual ~WinampMediaPlayer();

    // PlayerInfo
    QString getTitle();
    QString getAlbum();
    QString getArtist();
    QString getFile();
    int getLength();
    int getCurrentPos();
    bool isPlaying();
    bool isActive();
    QStringList getPlayListTitles();
    QStringList getPlayListFiles();
    uint getPlayListLength();
    QString getPlayerName();
    QString getPlayerVersion();

    // PlayerCommands
    void nextTrack();
    void prevTrack();
    void play();
    void stop();
    void pause();
    void setVolume(int vol);
    void incrVolume();
    void decrVolume();

private:
    HWND findWinamp();
    QString getFileImpl(int position);
    QString readWinampMemory(quint32 command, quint32 arg, bool unicode = false);
    QString getFileTagW(int position, QString tag);
    QString getFileTagA(int position, QString tag);
    HANDLE hProcess;
};
