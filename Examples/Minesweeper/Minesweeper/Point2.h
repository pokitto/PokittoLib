#pragma once

//
// Copyright (C) 2018 Pharap (@Pharap)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

template< typename T >
class Point2
{
public:
	T X;
	T Y;

public:
	Point2(void);
	Point2(const T & x, const T & y);
};

template< typename T >
bool operator ==(const Point2<T> & left, const Point2<T> & right)
{
	return
		left.X == right.X &&
		left.Y == right.Y;
}

template< typename T >
bool operator !=(const Point2<T> & left, const Point2<T> & right)
{
	return
		left.X != right.X ||
		left.Y != right.Y;
}


template< typename T >
Point2<T>::Point2(void)
	: X(), Y()
{
}

template< typename T >
Point2<T>::Point2(const T & x, const T & y)
	: X(x), Y(y)
{
}