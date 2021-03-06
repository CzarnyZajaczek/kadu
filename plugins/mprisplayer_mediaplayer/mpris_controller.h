/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/*
 * MPRIS standard implementation (used in Amarok2, Audacious, BMPx,
 * Dragon Player, VLC, XMMS2
 *
 * See http://mpris.org/ for more details about the standard
 */

#ifndef MPRISCONTROLLER_H
#define MPRISCONTROLLER_H

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QVariantMap>
#include <QtDBus/QDBusMessage>

#include "track-info.h"

// TODO: separate class to support DBus properies must be extracted

class MediaPlayer;

class MPRISController : public QObject
{
    Q_OBJECT

public:
    enum PlayerStatus
    {
        StatusPlaying,
        StatusPaused,
        StatusStopped
    };

private:
    QPointer<MediaPlayer> m_mediaPlayer;

    PlayerStatus CurrentStatus;
    TrackInfo CurrentTrack;

    bool Active;
    QString Service;

    void activate();
    void deactivate();

    void updateStatus(const PlayerStatus newStatus);
    void updateStatus(const QString &newStatus);

    TrackInfo toTrackInfo(const QVariantMap &metadata) const;
    void updateMetadata(const QVariantMap &metadata);

private slots:
    void nameOwnerChanged(const QString &service, const QString &previousOwner, const QString &currentOwner);
    void propertiesChanged(const QDBusMessage &message);

public:
    explicit MPRISController(MediaPlayer *mediaPlayer, const QString &service, QObject *parent = nullptr);
    virtual ~MPRISController();

    void call(const QString &methodName);

    QString identity() const;

    PlayerStatus status() const;
    const TrackInfo &track() const;
    bool active() const;

    void fetchStatus();
    void fetchMetadata();

    int getCurrentPosition() const;

    int getVolume() const;
    void setVolume(int volume) const;

    QList<TrackInfo> getTrackList() const;
};

#endif
