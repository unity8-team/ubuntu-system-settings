/*
 * Copyright (C) 2012,2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Michael Terry <michael.terry@canonical.com>
 *          Iain Lane <iain.lane@canonical.com>
 */

#ifndef POLKITLISTENER_H
#define POLKITLISTENER_H

#include <polkitagent/polkitagent.h>

G_BEGIN_DECLS

#define USS_TYPE_POLKIT_LISTENER         (uss_polkit_listener_get_type ())
#define USS_POLKIT_LISTENER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), USS_TYPE_POLKIT_LISTENER, UssPolkitListener))
#define USS_POLKIT_LISTENER_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), USS_TYPE_POLKIT_LISTENER, UssPolkitListenerClass))
#define USS_POLKIT_LISTENER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), USS_TYPE_POLKIT_LISTENER, UssPolkitListenerClass))
#define USS_IS_POLKIT_LISTENER(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), USS_TYPE_POLKIT_LISTENER))
#define USS_IS_POLKIT_LISTENER_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), USS_TYPE_POLKIT_LISTENER))

typedef struct _UssPolkitListener UssPolkitListener;
typedef struct _UssPolkitListenerClass UssPolkitListenerClass;
typedef struct _UssPolkitListenerPrivate UssPolkitListenerPrivate;

struct _UssPolkitListener
{
    PolkitAgentListener parent_instance;
    UssPolkitListenerPrivate *priv;
};

struct _UssPolkitListenerClass
{
    PolkitAgentListenerClass parent_class;
};

GType uss_polkit_listener_get_type(void) G_GNUC_CONST;

UssPolkitListener *uss_polkit_listener_new(void);

void uss_polkit_listener_free(UssPolkitListener *listener);

bool uss_polkit_listener_register(UssPolkitListener *listener);

void uss_polkit_listener_set_password(UssPolkitListener *listener, const gchar *password);

void uss_polkit_listener_set_pid(UssPolkitListener *listener, int pid);

bool uss_polkit_listener_run(UssPolkitListener *listener);

G_END_DECLS

#endif //POLKITLISTENER_H
