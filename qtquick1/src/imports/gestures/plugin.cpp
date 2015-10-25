/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtDeclarative/qdeclarativeextensionplugin.h>
#include <QtDeclarative/qdeclarative.h>

#include "qdeclarativegesturearea_p.h"

QT_BEGIN_NAMESPACE

class GestureAreaQmlPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDeclarativeExtensionInterface" FILE "gestures.json")

public:
    virtual void registerTypes(const char *uri)
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Qt.labs.gestures"));
#ifndef QT_NO_GESTURES
        qmlRegisterCustomType<QDeclarativeGestureArea>(uri,1,0, "GestureArea", new QDeclarativeGestureAreaParser);

        qmlRegisterUncreatableType<QGesture>(uri, 1, 0, "Gesture", QLatin1String("Do not create objects of this type."));
        qmlRegisterUncreatableType<QPanGesture>(uri, 1, 0, "PanGesture", QLatin1String("Do not create objects of this type."));
        qmlRegisterUncreatableType<QTapGesture>(uri, 1, 0, "TapGesture", QLatin1String("Do not create objects of this type."));
        qmlRegisterUncreatableType<QTapAndHoldGesture>(uri, 1, 0, "TapAndHoldGesture", QLatin1String("Do not create objects of this type."));
        qmlRegisterUncreatableType<QPinchGesture>(uri, 1, 0, "PinchGesture", QLatin1String("Do not create objects of this type."));
        qmlRegisterUncreatableType<QSwipeGesture>(uri, 1, 0, "SwipeGesture", QLatin1String("Do not create objects of this type."));
#endif
    }
};

QT_END_NAMESPACE

#include "plugin.moc"