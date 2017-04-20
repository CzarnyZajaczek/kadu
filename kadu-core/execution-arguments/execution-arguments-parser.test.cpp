/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "execution-arguments/execution-arguments-parser.h"
#include "execution-arguments/execution-arguments.h"

#include <QtTest/QtTest>
#include <vector>

class ExcutionArgumentsParserTest : public QObject
{
    Q_OBJECT

private slots:
    void shouldCreateDefaultArgumentsWhenNonePassed();
    void shouldProperlyParseQueryVersion();
    void shouldProperlyParseQueryUsage();
    void shouldProperlyParseProfileDirectory();
    void shouldIgnoreEmptyProfileDirectory();
    void shouldAcceptLastProfileDirectory();
    void shouldProperlyParseListOfIds();
    void shouldIgnoreInvalidIds();
    void shouldProperlyParseComplexArguments();
};

void ExcutionArgumentsParserTest::shouldCreateDefaultArgumentsWhenNonePassed()
{
    auto arguments = QStringList{};
    auto parser = ExecutionArgumentsParser{};
    auto executionArguments = parser.parse(arguments);

    QCOMPARE(executionArguments.queryVersion(), false);
    QCOMPARE(executionArguments.queryUsage(), false);
    QCOMPARE(executionArguments.profileDirectory(), QString{});
    QCOMPARE(executionArguments.openIds(), QStringList{});
}

void ExcutionArgumentsParserTest::shouldProperlyParseQueryVersion()
{
    auto arguments = QStringList{} << "--version";
    auto parser = ExecutionArgumentsParser{};
    auto executionArguments = parser.parse(arguments);

    QCOMPARE(executionArguments.queryVersion(), true);
    QCOMPARE(executionArguments.queryUsage(), false);
    QCOMPARE(executionArguments.profileDirectory(), QString{});
    QCOMPARE(executionArguments.openIds(), QStringList{});
}

void ExcutionArgumentsParserTest::shouldProperlyParseQueryUsage()
{
    auto arguments = QStringList{} << "--help";
    auto parser = ExecutionArgumentsParser{};
    auto executionArguments = parser.parse(arguments);

    QCOMPARE(executionArguments.queryVersion(), false);
    QCOMPARE(executionArguments.queryUsage(), true);
    QCOMPARE(executionArguments.profileDirectory(), QString{});
    QCOMPARE(executionArguments.openIds(), QStringList{});
}

void ExcutionArgumentsParserTest::shouldProperlyParseProfileDirectory()
{
    auto arguments = QStringList{} << "--config-dir"
                                   << "kadu-2";
    auto parser = ExecutionArgumentsParser{};
    auto executionArguments = parser.parse(arguments);

    QCOMPARE(executionArguments.queryVersion(), false);
    QCOMPARE(executionArguments.queryUsage(), false);
    QCOMPARE(executionArguments.profileDirectory(), QString{"kadu-2"});
    QCOMPARE(executionArguments.openIds(), QStringList{});
}

void ExcutionArgumentsParserTest::shouldIgnoreEmptyProfileDirectory()
{
    auto arguments = QStringList{} << "--config-dir";
    auto parser = ExecutionArgumentsParser{};
    auto executionArguments = parser.parse(arguments);

    QCOMPARE(executionArguments.queryVersion(), false);
    QCOMPARE(executionArguments.queryUsage(), false);
    QCOMPARE(executionArguments.profileDirectory(), QString{});
    QCOMPARE(executionArguments.openIds(), QStringList{});
}

void ExcutionArgumentsParserTest::shouldAcceptLastProfileDirectory()
{
    auto arguments = QStringList{} << "--config-dir"
                                   << "kadu-2"
                                   << "--config-dir"
                                   << "kadu-3";
    auto parser = ExecutionArgumentsParser{};
    auto executionArguments = parser.parse(arguments);

    QCOMPARE(executionArguments.queryVersion(), false);
    QCOMPARE(executionArguments.queryUsage(), false);
    QCOMPARE(executionArguments.profileDirectory(), QString{"kadu-3"});
    QCOMPARE(executionArguments.openIds(), QStringList{});
}

void ExcutionArgumentsParserTest::shouldProperlyParseListOfIds()
{
    auto arguments = QStringList{} << "gg:123"
                                   << "gg:456"
                                   << "xmpp:test@example.com";
    auto parser = ExecutionArgumentsParser{};
    auto executionArguments = parser.parse(arguments);

    QCOMPARE(executionArguments.queryVersion(), false);
    QCOMPARE(executionArguments.queryUsage(), false);
    QCOMPARE(executionArguments.profileDirectory(), QString{});
    QCOMPARE(
        executionArguments.openIds(),
        QStringList{} << "gg:123"
                      << "gg:456"
                      << "xmpp:test@example.com");
}

void ExcutionArgumentsParserTest::shouldIgnoreInvalidIds()
{
    auto arguments = QStringList{} << "gg:123"
                                   << "gg:"
                                   << "gg:456"
                                   << "xmpp:test@example.com"
                                   << "xmpp:"
                                   << "no-id";
    auto parser = ExecutionArgumentsParser{};
    auto executionArguments = parser.parse(arguments);

    QCOMPARE(executionArguments.queryVersion(), false);
    QCOMPARE(executionArguments.queryUsage(), false);
    QCOMPARE(executionArguments.profileDirectory(), QString{});
    QCOMPARE(
        executionArguments.openIds(),
        QStringList{} << "gg:123"
                      << "gg:456"
                      << "xmpp:test@example.com");
}

void ExcutionArgumentsParserTest::shouldProperlyParseComplexArguments()
{
    auto arguments = QStringList{} << "--version"
                                   << "--help"
                                   << "--config-dir"
                                   << "kadu-7"
                                   << "gg:15"
                                   << "xmpp:12";
    auto parser = ExecutionArgumentsParser{};
    auto executionArguments = parser.parse(arguments);

    QCOMPARE(executionArguments.queryVersion(), true);
    QCOMPARE(executionArguments.queryUsage(), true);
    QCOMPARE(executionArguments.profileDirectory(), QString{"kadu-7"});
    QCOMPARE(
        executionArguments.openIds(),
        QStringList{} << "gg:15"
                      << "xmpp:12");
}

QTEST_APPLESS_MAIN(ExcutionArgumentsParserTest)
#include "execution-arguments-parser.test.moc"
