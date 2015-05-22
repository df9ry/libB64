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

#ifndef B64CODER_H_
#define B64CODER_H_

#include <sstream>
#include <iostream>
#include <vector>
#include <memory>

namespace B64 {

	/**
	 * Encode a binary input stream into a base64 string. Input and
	 * output are supplied by lambdas for maximum flexibility.
	 * @param rd Read lambda. This function shall return -1 if no
	 *           more data is available. Otherwise the value of the
	 *           next byte shall be returned.
	 * @param wr This lambda shall write the next char to the output.
	 */
	void encode(std::function<int()> rd, std::function<void(char)> wr);

	/**
	 * Encode a binary input stream into a base64 string stream.
	 * @param in The binary input stream
	 * @param out The encoded base64 string stream
	 */
	void encode(std::istream& in, std::ostringstream& out);

	/**
	 * Encode binary data into a base64 coded string.
	 * @param data
	 * @return
	 */
	std::string encode(const std::vector<uint8_t>& data);

	/**
	 * Decode a base64 coded input stream into a byte sequence.
	 * @param rd This lambda shall return -1 if no more input is
	 *           available. Otherwise it shall return the next char.
	 * @param wr This lambda shall write the next byte into the
	 *           output stream.
	 */
	void decode(std::function<int()> rd, std::function<void(uint8_t)> wr);

	/**
	 * Decode a base64 string stream into a binary output stream
	 * @param in Base64 encoded string stream
	 * @param out Binary output stream
	 */
	void decode(std::istringstream& in, std::ostream& out);

	/**
	 * Encode a base64 coded string to a binary vector.
	 * @param s The base64 encoded string
	 * @return Data vector as a moved unique pointer
	 */
	std::unique_ptr<std::vector<uint8_t>>&&
		decode(const std::string& s);

} // end namespace B64 //

#endif /* B64CODER_H_ */
