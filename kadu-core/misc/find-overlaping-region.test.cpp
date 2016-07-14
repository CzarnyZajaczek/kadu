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

#include "misc/algorithm.h"

#include <vector>
#include <QtTest/QtTest>

class FindOverlapingRegionTest : public QObject
{
	Q_OBJECT

private slots:
	void shouldFindWholeRegionWhenBothRangesEmpty();
	void shouldFindNoRegionWhenFirstRangeEmpty();
	void shouldFindNoRegionWhenSecondRangeEmpty();
	void shouldFindWholeRegionWhenBothRangesEqual();
	void shouldFindSecondRangeWhenSuffixOfFirst();
	void shouldFindNoRangeWhenFirstSuffixOfSecond();
	void shouldFindFirstRangeWhenPreffixOfSecond();
	void shouldFindNoRangeWhenSecondPrefixOfFirst();
	void shouldFindCommonRegionWhenExists();
	void shouldFindNoRegionWhenTotallyDifferent();
	void shouldFindNoRegionWhenDifferenceInMiddle();

};

void FindOverlapingRegionTest::shouldFindWholeRegionWhenBothRangesEmpty()
{
	auto left = std::vector<int>{};
	auto right = std::vector<int>{};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, begin(left));
	QCOMPARE(overlapping.first, end(left));
	QCOMPARE(overlapping.second, begin(right));
	QCOMPARE(overlapping.second, end(right));
}

void FindOverlapingRegionTest::shouldFindNoRegionWhenFirstRangeEmpty()
{
	auto left = std::vector<int>{};
	auto right = std::vector<int>{1, 2, 3};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, begin(left));
	QCOMPARE(overlapping.first, end(left));
	QCOMPARE(overlapping.second, begin(right));
}

void FindOverlapingRegionTest::shouldFindNoRegionWhenSecondRangeEmpty()
{
	auto left = std::vector<int>{1, 2, 3};
	auto right = std::vector<int>{};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, end(left));
	QCOMPARE(overlapping.second, begin(right));
	QCOMPARE(overlapping.second, end(right));
}

void FindOverlapingRegionTest::shouldFindWholeRegionWhenBothRangesEqual()
{
	auto left = std::vector<int>{1, 2, 3};
	auto right = std::vector<int>{1, 2, 3};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, begin(left));
	QCOMPARE(overlapping.second, end(right));
}

void FindOverlapingRegionTest::shouldFindSecondRangeWhenSuffixOfFirst()
{
	auto left = std::vector<int>{1, 2, 3};
	auto right = std::vector<int>{2, 3};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, begin(left) + 1);
	QCOMPARE(overlapping.second, end(right));
}

void FindOverlapingRegionTest::shouldFindNoRangeWhenFirstSuffixOfSecond()
{
	auto left = std::vector<int>{2, 3};
	auto right = std::vector<int>{1, 2, 3};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, end(left));
	QCOMPARE(overlapping.second, begin(right));
}

void FindOverlapingRegionTest::shouldFindFirstRangeWhenPreffixOfSecond()
{
	auto left = std::vector<int>{1, 2, 3};
	auto right = std::vector<int>{1, 2, 3, 4};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, begin(left));
	QCOMPARE(overlapping.second, end(right) - 1);
}

void FindOverlapingRegionTest::shouldFindNoRangeWhenSecondPrefixOfFirst()
{
	auto left = std::vector<int>{1, 2, 3, 4};
	auto right = std::vector<int>{1, 2, 3};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, end(left));
	QCOMPARE(overlapping.second, begin(right));
}

void FindOverlapingRegionTest::shouldFindCommonRegionWhenExists()
{
	auto left = std::vector<int>{1, 2, 3};
	auto right = std::vector<int>{2, 3, 4};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, begin(left) + 1);
	QCOMPARE(overlapping.second, end(right) - 1);
}

void FindOverlapingRegionTest::shouldFindNoRegionWhenTotallyDifferent()
{
	auto left = std::vector<int>{1, 2, 3};
	auto right = std::vector<int>{4, 5, 6};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, end(left));
	QCOMPARE(overlapping.second, begin(right));
}

void FindOverlapingRegionTest::shouldFindNoRegionWhenDifferenceInMiddle()
{
	auto left = std::vector<int>{1, 2, 3, 5, 6, 7};
	auto right = std::vector<int>{1, 2, 3, 4, 6, 7};
	auto overlapping = find_overlapping_region(begin(left), end(left), begin(right), end(right));

	QCOMPARE(overlapping.first, end(left));
	QCOMPARE(overlapping.second, begin(right));
}

QTEST_APPLESS_MAIN(FindOverlapingRegionTest)
#include "find-overlaping-region.test.moc"
