/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include <algorithm>
#include <map>

template<typename C, typename T>
bool contains(const C &c, const T &v)
{
	return std::find(c.begin(), c.end(), v) != c.end();
}

template<typename K, typename V>
bool contains(const std::map<K, V> &c, const K &k)
{
	return c.find(k) != c.end();
}

template<typename C, typename T>
bool precedes(const C &c, const T &v1, const T &v2)
{
	auto it1 = std::find(c.begin(), c.end(), v1);
	auto it2 = std::find(c.begin(), c.end(), v2);

	return it1 < it2;
}