/*
 * Copyright (C) 2015 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Jonas G. Drange <jonas.drange@canonical.com>
 *
 */

#include <QDebug>
#include <QQmlEngine>

#include "displays.h"

Displays::Displays(QObject *parent) :
    QObject(parent),
    m_displaysModel(this),
    m_mirDisplays(this) {

    if (m_mirDisplays.isConnected()) {
        updateAvailableMirDisplays();

        connect(&m_mirDisplays, SIGNAL(configurationChanged()),
                    this, SLOT(updateAvailableMirDisplays()));
    }
}

Displays::~Displays() {}

QAbstractItemModel * Displays::displays() {
    auto ret = &m_displaysModel;
    QQmlEngine::setObjectOwnership(ret, QQmlEngine::CppOwnership);
    return ret;
}

void Displays::applyDisplayConfiguration() {
    qWarning() << "apply config";
    MirDisplayOutput * outs[m_displaysModel.rowCount()] = {};
    MirDisplayConfiguration * conf = m_mirDisplays.getConfiguration();

    for (int i = 0; i < m_displaysModel.rowCount(); i++) {
        int x = (int)i;
        QModelIndex ix = m_displaysModel.index(x, 0, QModelIndex());
        QString name = m_displaysModel.data(ix).toString();
        QSharedPointer<Display> display = m_displaysModel.getDisplay(name);
        outs[i] = display->output();
    }

    for (unsigned  int i = 0; i < conf->num_outputs; ++i) {
        MirDisplayOutput output = conf->outputs[i];
        qWarning() << "output" << i << "used" << output.used << "mode" << output.current_mode;

    }
    qWarning() << "sat new outputs";
    conf->outputs = reinterpret_cast<MirDisplayOutput*>(outs);

    for (unsigned int i = 0; i < conf->num_outputs; ++i) {
        MirDisplayOutput output = conf->outputs[i];
        qWarning() << "output" << i << "used" << output.used << "mode" << output.current_mode;
    }

    m_mirDisplays.applyConfiguration(conf);
}

void Displays::updateAvailableMirDisplays() {
    MirDisplayConfiguration *conf = m_mirDisplays.getConfiguration();
    for (unsigned int i = 0; i < conf->num_outputs; ++i) {
        MirDisplayOutput output = conf->outputs[i];
        qWarning() << "output" << i;
        QSharedPointer<Display> display(new Display(&output));
        m_displaysModel.addDisplay(display);
    }
}

