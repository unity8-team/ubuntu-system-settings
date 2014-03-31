/*
 * This file is part of system-settings
 *
 * Copyright (C) 2014 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PageList.h"

#include <QDebug>
#include <QObject>
#include <QTemporaryDir>
#include <QTest>

#define PAGES_PATH "ubuntu/settings/wizard/qml/Pages"

class PageListTest: public QObject
{
    Q_OBJECT

public:
    PageListTest() {};

private Q_SLOTS:
    void testCollect();
    void testIterate();
    void testIgnoreNonNumbered();
    void testIgnoreDuplicates();
    void testDisabled();

private:
    void fillRoot(const QTemporaryDir &root);
    void makeFile(const QTemporaryDir &root, const QString &dir, const QString &filename);
};

void PageListTest::fillRoot(const QTemporaryDir &root)
{
    QVERIFY(root.isValid());
    QDir rootDir = root.path();
    QVERIFY(rootDir.mkpath(QString("a/") + PAGES_PATH));
    QVERIFY(rootDir.mkpath(QString("b/") + PAGES_PATH));
    QVERIFY(rootDir.mkpath(QString("c/") + PAGES_PATH));
    qputenv("XDG_DATA_DIRS", QString(rootDir.path() + "/a:" +
                                     rootDir.path() + "/b:" +
                                     rootDir.path() + "/c").toLatin1());
}

void PageListTest::makeFile(const QTemporaryDir &root, const QString &dir, const QString &filename)
{
    QFile file(root.path() + "/" + dir + "/" + PAGES_PATH + "/" + filename);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();
    QVERIFY(file.exists());
}

void PageListTest::testCollect()
{
    QTemporaryDir root;
    fillRoot(root);
    makeFile(root, "a", "3");
    makeFile(root, "b", "1");
    makeFile(root, "c", "2");

    PageList pageList;
    QCOMPARE(pageList.entries(), QStringList() << "1" << "2" << "3");
    QCOMPARE(pageList.paths(), QStringList() << root.path() + "/b/" + PAGES_PATH + "/1"
                                             << root.path() + "/c/" + PAGES_PATH + "/2"
                                             << root.path() + "/a/" + PAGES_PATH + "/3");
}

void PageListTest::testIterate()
{
    QTemporaryDir root;
    fillRoot(root);
    makeFile(root, "a", "1");
    makeFile(root, "a", "2");
    makeFile(root, "a", "3");

    PageList pageList;
    QCOMPARE(pageList.index(), -1);
    QCOMPARE(pageList.next(), root.path() + "/a/" + PAGES_PATH + "/1");
    QCOMPARE(pageList.prev(), QString());
    QCOMPARE(pageList.next(), root.path() + "/a/" + PAGES_PATH + "/2");
    QCOMPARE(pageList.prev(), root.path() + "/a/" + PAGES_PATH + "/1");
    QCOMPARE(pageList.index(), 0);
    QCOMPARE(pageList.next(), root.path() + "/a/" + PAGES_PATH + "/2");
    QCOMPARE(pageList.next(), root.path() + "/a/" + PAGES_PATH + "/3");
    QCOMPARE(pageList.index(), 2);
    QCOMPARE(pageList.next(), QString());
    QCOMPARE(pageList.index(), 2);
}

void PageListTest::testIgnoreNonNumbered()
{
    QTemporaryDir root;
    fillRoot(root);
    makeFile(root, "a", "1");
    makeFile(root, "a", "nope");

    PageList pageList;
    QCOMPARE(pageList.entries(), QStringList() << "1");
}

void PageListTest::testIgnoreDuplicates()
{
    QTemporaryDir root;
    fillRoot(root);
    makeFile(root, "a", "1");
    makeFile(root, "b", "1");

    PageList pageList;
    QCOMPARE(pageList.paths(), QStringList() << root.path() + "/a/" + PAGES_PATH + "/1");
}

void PageListTest::testDisabled()
{
    QTemporaryDir root;
    fillRoot(root);
    makeFile(root, "a", "1.disabled"); // before the fact
    makeFile(root, "b", "1");
    makeFile(root, "b", "2");
    makeFile(root, "b", "2.disabled"); // same dir
    makeFile(root, "b", "3");
    makeFile(root, "b", "4"); // only survivor
    makeFile(root, "c", "3.disabled"); // after the fact

    PageList pageList;
    QCOMPARE(pageList.entries(), QStringList() << "4");
}

QTEST_MAIN(PageListTest)
#include "tst_pagelist.moc"
