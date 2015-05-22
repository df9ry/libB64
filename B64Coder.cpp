/*
    Project libB64
    Copyright (C) 2015  tania@df9ry.de

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "B64Coder.h"

using namespace std;

namespace B64 {

static constexpr char encTable[65] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static constexpr char decTable[256] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21 ,22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

void encode(function<int()> rd, function<void(char)> wr) {
	char buff1[3];
	char buff2[4];
	uint8_t i = 0, j;
	int  ch;
	while ((ch = rd()) != -1) {
		buff1[i++] = static_cast<char>(ch);
		if (i == 3) {
			wr(encTable[(buff1[0] & 0xfc) >> 2]);
			wr(encTable[((buff1[0] & 0x03) << 4) + ((buff1[1] & 0xf0) >> 4)]);
			wr(encTable[((buff1[1] & 0x0f) << 2) + ((buff1[2] & 0xc0) >> 6)]);
			wr(encTable[buff1[2] & 0x3f]);
			i = 0;
		}
	} // end while //

	if (--i) {
		for(j = i; j < 3; j++) buff1[j] = '\0';

		buff2[0] = (buff1[0] & 0xfc) >> 2;
		buff2[1] = ((buff1[0] & 0x03) << 4) + ((buff1[1] & 0xf0) >> 4);
		buff2[2] = ((buff1[1] & 0x0f) << 2) + ((buff1[2] & 0xc0) >> 6);
		buff2[3] = buff1[2] & 0x3f;

		for (j = 0; j < (i+1); j++) wr(encTable[static_cast<uint8_t>(buff2[j])]);

		while(i++ < 3) wr('=');
	}
}

void encode(istream &in, ostringstream &out)
{
	encode([&in](){ char b; return in.readsome(&b, 1) ? b : -1; },
			[&out](char ch){ out << ch; });
}

string encode(const vector<uint8_t>& v) {
	vector<uint8_t>::const_iterator iter = v.begin();
	ostringstream oss{};
	encode([&v, &iter](){ return (iter != v.end()) ? *(iter++) : -1; },
			[&oss](char ch){ oss << ch; });
	return oss.str();
}

void decode(function<int()> rd, function<void(uint8_t)> wr) {
	char buff1[4];
	char buff2[4];
	uint8_t i=0, j;
	int ch;

	while(((ch = rd()) != -1) && (ch != '=')) {
		buff2[i] = static_cast<char>(ch);
		if (++i==4)
		{
			for (i=0;i!=4;i++)
				buff2[i] = decTable[static_cast<int>(buff2[i])];

			wr((char)((buff2[0] << 2) + ((buff2[1] & 0x30) >> 4)));
			wr((char)(((buff2[1] & 0xf) << 4) + ((buff2[2] & 0x3c) >> 2)));
			wr((char)(((buff2[2] & 0x3) << 6) + buff2[3]));

			i = 0;
		}
	} // end while //

	if (i) {
		for (j=i;j<4;j++) buff2[j] = '\0';
		for (j=0;j<4;j++) buff2[j] = decTable[static_cast<int>(buff2[j])];

		buff1[0] = (buff2[0] << 2) + ((buff2[1] & 0x30) >> 4);
		buff1[1] = ((buff2[1] & 0xf) << 4) + ((buff2[2] & 0x3c) >> 2);
		buff1[2] = ((buff2[2] & 0x3) << 6) + buff2[3];

		for (j=0;j<(i-1); j++) wr((char)buff1[j]);
	}
}

void decode(istringstream &in, ostream &out)
{
	decode([&in](){ char b; return in.readsome(&b, 1) ? b : -1; },
			[&out](uint8_t x){ out << x; });
}

unique_ptr<vector<uint8_t>>&& decode(const string& s) {
	string::const_iterator iter = s.begin();
	auto pv = new vector<uint8_t>();
	encode([&s, &iter](){ return (iter != s.end()) ? *(iter++) : -1; },
		   [&pv](uint8_t x){ pv->push_back(x); });
	return move(unique_ptr<vector<uint8_t>>{pv});
}

} // end namespace B64
