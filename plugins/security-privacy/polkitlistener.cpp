/*
 * Copyright (C) 2013,2014 Canonical, Ltd.
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
 * Author: Michael Terry <michael.terry@canonical.com>
 *         Iain Lane <iain.lane@canonical.com>
 */

#include "polkitlistener.h"

struct _UssPolkitListenerPrivate
{
    GMainLoop *loop;
    PolkitAgentSession *session;
    GSimpleAsyncResult *simple;
    gpointer registration;
    int pid;
    gchar *password;
    gboolean successful;
};

#define USS_POLKIT_LISTENER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE((o), USS_TYPE_POLKIT_LISTENER, UssPolkitListenerPrivate))

static void uss_polkit_listener_initiate_authentication(PolkitAgentListener *_listener,
                                                        const gchar         *action_id,
                                                        const gchar         *message,
                                                        const gchar         *icon_name,
                                                        PolkitDetails       *details,
                                                        const gchar         *cookie,
                                                        GList               *identities,
                                                        GCancellable        *cancellable,
                                                        GAsyncReadyCallback  callback,
                                                        gpointer             user_data);

static gboolean uss_polkit_listener_initiate_authentication_finish(PolkitAgentListener  *_listener,
                                                                   GAsyncResult         *res,
                                                                   GError              **error);

G_DEFINE_TYPE(UssPolkitListener, uss_polkit_listener, POLKIT_AGENT_TYPE_LISTENER)

static void
uss_polkit_listener_init(UssPolkitListener *listener)
{
    listener->priv = USS_POLKIT_LISTENER_GET_PRIVATE(listener);
}

static void
uss_polkit_listener_dispose(GObject *object)
{
    UssPolkitListener *listener = USS_POLKIT_LISTENER(object);
    UssPolkitListenerPrivate *priv = listener->priv;

    g_clear_object(&priv->session);

    G_OBJECT_CLASS(uss_polkit_listener_parent_class)->dispose(object);
}

static void
uss_polkit_listener_finalize(GObject *object)
{
    UssPolkitListener *listener = USS_POLKIT_LISTENER(object);
    UssPolkitListenerPrivate *priv = listener->priv;

    if (priv->password != nullptr)
        g_free(priv->password);

    G_OBJECT_CLASS(uss_polkit_listener_parent_class)->finalize(object);
}

static void
uss_polkit_listener_class_init(UssPolkitListenerClass *klass)
{
    GObjectClass *gobject_class;
    PolkitAgentListenerClass *listener_class;

    gobject_class = G_OBJECT_CLASS(klass);
    g_type_class_add_private(gobject_class, sizeof(UssPolkitListenerPrivate));
    gobject_class->dispose = uss_polkit_listener_dispose;
    gobject_class->finalize = uss_polkit_listener_finalize;

    listener_class = POLKIT_AGENT_LISTENER_CLASS(klass);
    listener_class->initiate_authentication = uss_polkit_listener_initiate_authentication;
    listener_class->initiate_authentication_finish = uss_polkit_listener_initiate_authentication_finish;
}

UssPolkitListener *
uss_polkit_listener_new(void)
{
    return USS_POLKIT_LISTENER(g_object_new(USS_TYPE_POLKIT_LISTENER, nullptr));
}

bool uss_polkit_listener_register(UssPolkitListener *listener)
{
    UssPolkitListenerPrivate *priv = listener->priv;

    PolkitSubject *subject = polkit_unix_process_new_for_owner(priv->pid, 0, getuid());
    priv->registration = polkit_agent_listener_register(POLKIT_AGENT_LISTENER(listener),
                                                        POLKIT_AGENT_REGISTER_FLAGS_RUN_IN_THREAD,
                                                        subject, nullptr, nullptr, nullptr);
    g_object_unref(subject);
    if (priv->registration == nullptr) {
        g_object_unref(listener);
        return false;
    }

    return true;
}

void uss_polkit_listener_free(UssPolkitListener *listener)
{
    UssPolkitListenerPrivate *priv = listener->priv;
    if (priv->registration != NULL) {
        polkit_agent_listener_unregister(priv->registration);
    }
    g_object_unref(listener);
}

void uss_polkit_listener_set_password(UssPolkitListener *listener, const gchar *password)
{
    UssPolkitListenerPrivate *priv = listener->priv;
    priv->password = g_strdup(password);
}

void uss_polkit_listener_set_pid(UssPolkitListener *listener, int pid)
{
    UssPolkitListenerPrivate *priv = listener->priv;
    priv->pid = pid;
}

bool uss_polkit_listener_run(UssPolkitListener *listener)
{
    UssPolkitListenerPrivate *priv = listener->priv;
    if (priv->loop != nullptr)
        return false;

    priv->loop = g_main_loop_new(nullptr, FALSE);
    g_main_loop_run(priv->loop);

    return priv->successful;
}

static void
on_completed(PolkitAgentSession */*session*/,
             gboolean            gained_authorization,
             gpointer            user_data)
{
    UssPolkitListener *listener = USS_POLKIT_LISTENER(user_data);
    UssPolkitListenerPrivate *priv = listener->priv;

    priv->successful = gained_authorization;
    g_simple_async_result_complete_in_idle(priv->simple);

    g_clear_object(&priv->simple);
    g_clear_object(&priv->session);
    g_free(priv->password);
    priv->password = nullptr;

    if (priv->loop != nullptr)
        g_main_loop_quit(priv->loop);
}

static void
on_request(PolkitAgentSession *session,
           const gchar        */*request*/,
           gboolean            /*echo_on*/,
           gpointer            user_data)
{
    UssPolkitListener *listener = USS_POLKIT_LISTENER(user_data);
    UssPolkitListenerPrivate *priv = listener->priv;
    polkit_agent_session_response(session, priv->password);
}

static void
uss_polkit_listener_initiate_authentication(PolkitAgentListener  *agent_listener,
                                            const gchar          */*action_id*/,
                                            const gchar          */*message*/,
                                            const gchar          */*icon_name*/,
                                            PolkitDetails        */*details*/,
                                            const gchar          *cookie,
                                            GList                *identities,
                                            GCancellable         */*cancellable*/,
                                            GAsyncReadyCallback   callback,
                                            gpointer              user_data)
{
    UssPolkitListener *listener = USS_POLKIT_LISTENER(agent_listener);
    UssPolkitListenerPrivate *priv = listener->priv;
    GSimpleAsyncResult *simple;
    PolkitIdentity *identity;
    GList *iter;

    simple = g_simple_async_result_new(G_OBJECT(listener),
                                       callback,
                                       user_data,
                                       (gpointer)uss_polkit_listener_initiate_authentication);
    if (priv->session != nullptr) {
        g_simple_async_result_set_error(simple, POLKIT_ERROR,
                                        POLKIT_ERROR_FAILED,
                                        "Already one authentication in progress");
        g_simple_async_result_complete_in_idle(simple);
        g_object_unref(simple);
        return;
    }

    for (iter = identities; iter; iter = iter->next) {
        identity = POLKIT_IDENTITY(iter->data);
        if (POLKIT_IS_UNIX_USER(identity) &&
            (uid_t)polkit_unix_user_get_uid(POLKIT_UNIX_USER(identity)) == geteuid())
            break;
    }
    if (!iter) {
        g_simple_async_result_set_error(simple, POLKIT_ERROR,
                                        POLKIT_ERROR_FAILED,
                                        "Could not find current user identity");
        g_simple_async_result_complete_in_idle(simple);
        g_object_unref(simple);
        return;
    }

    priv->simple = simple;
    priv->session = polkit_agent_session_new(identity, cookie);
    g_signal_connect(priv->session, "completed",
                     G_CALLBACK(on_completed), listener);
    g_signal_connect(priv->session, "request",
                     G_CALLBACK(on_request), listener);

    polkit_agent_session_initiate(priv->session);
}

static gboolean
uss_polkit_listener_initiate_authentication_finish(PolkitAgentListener  */*listener*/,
                                                   GAsyncResult         *res,
                                                   GError              **error)
{
    if (g_simple_async_result_propagate_error(G_SIMPLE_ASYNC_RESULT(res), error))
        return FALSE;
    else
        return TRUE;
}
