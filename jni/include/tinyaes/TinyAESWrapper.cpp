// TinyAES++ -- AES encryption/decryption for C and C++
// Public domain; see http://unlicense.org/
// Stephane Charette <stephanecharette@gmail.com>
// $Id: TinyAES.cpp 2422 2017-12-12 21:57:08Z stephane $
#ifndef TINYAES_H
#define TINYAES_H

#include "TinyAES.h"
#include <random>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <string>
#include <vector>
#include <cstdint>

namespace Amosoft
{
	class TinyAESWrapper
	{
	private:
	public:

 		typedef std::vector<uint8_t> VBytes;
 		const size_t key_size_in_bytes = 32;
		const size_t iv_size_in_bytes = 16;
		void validate_key_and_iv(const VBytes &key, const VBytes &iv)
		{
			if (key.size() != key_size_in_bytes)
			{
				/// @throw std::length_error Throws if the key isn't exactly 32 bytes long.
				throw std::length_error("expected encryption key size to be " + std::to_string(key_size_in_bytes) + " bytes, but instead found key to be " + std::to_string(key.size()) + " bytes");
			}

			if (iv.size() != iv_size_in_bytes)
			{
				/// @throw std::length_error Throws if the iv isn't exactly 16 bytes long.
				throw std::length_error("expected iv size to be " + std::to_string(iv_size_in_bytes) + " bytes, but instead found iv to be " + std::to_string(iv.size()) + " bytes");
			}

			return;
		}


		void generate_random_iv(VBytes &iv, size_t seed = 0)
		{
			iv.clear();
			iv.reserve(iv_size_in_bytes);

			std::independent_bits_engine<std::default_random_engine, 8, uint8_t> ibe;

			/* If this method is called multiple times in a row, we need to try and
			 * use a different seed value at each call.  Otherwise, we end up with
			 * duplicate keys and initialization vectors.  We can help achieve unique
			 * seeds by keeping a random primer from the previous call to this
			 * function.  The primer is added to the time-based seed on the next call
			 * so that two calls within the same time frequency don't result in
			 * identical seeds.
			 */
			static size_t primer = 0;
			if (seed == 0)
			{
				auto t = std::chrono::high_resolution_clock::now();
				seed = primer + std::chrono::duration_cast<std::chrono::nanoseconds>(t.time_since_epoch()).count();
			}
			ibe.seed(seed);

			for (size_t count = 0; count < iv_size_in_bytes; count++)
			{
				iv.push_back(ibe());
			}
			iv.shrink_to_fit();
			// get a new primer for the next time this method is called
			primer += ibe();

			return;
		}

		void generate_random_key_and_iv(VBytes &key, VBytes &iv, size_t seed = 0)
		{
			key.clear();
			key.reserve(key_size_in_bytes);

			iv.clear();
			iv.reserve(iv_size_in_bytes);

			std::independent_bits_engine<std::default_random_engine, 8, uint8_t> ibe;

			/* If this method is called multiple times in a row, we need to try and
			 * use a different seed value at each call.  Otherwise, we end up with
			 * duplicate keys and initialization vectors.  We can help achieve unique
			 * seeds by keeping a random primer from the previous call to this
			 * function.  The primer is added to the time-based seed on the next call
			 * so that two calls within the same time frequency don't result in
			 * identical seeds.
			 */
			static size_t primer = 0;
			if (seed == 0)
			{
				auto t = std::chrono::high_resolution_clock::now();
				seed = primer + std::chrono::duration_cast<std::chrono::nanoseconds>(t.time_since_epoch()).count();
			}
			ibe.seed(seed);

			for (size_t count = 0; count < key_size_in_bytes; count++)
			{
				key.push_back(ibe());
			}
			key.shrink_to_fit();

			for (size_t count = 0; count < iv_size_in_bytes; count++)
			{
				iv.push_back(ibe());
			}
			iv.shrink_to_fit();

			validate_key_and_iv(key, iv);

			// get a new primer for the next time this method is called
			primer += ibe();

			return;
		}

		void generate_random(VBytes &data, size_t randomSize, size_t seed = 0)
		{
			data.clear();
			data.reserve(randomSize);



			std::independent_bits_engine<std::default_random_engine, 8, uint8_t> ibe;

			/* If this method is called multiple times in a row, we need to try and
			 * use a different seed value at each call.  Otherwise, we end up with
			 * duplicate keys and initialization vectors.  We can help achieve unique
			 * seeds by keeping a random primer from the previous call to this
			 * function.  The primer is added to the time-based seed on the next call
			 * so that two calls within the same time frequency don't result in
			 * identical seeds.
			 */
			static size_t primer = 0;
			if (seed == 0)
			{
				auto t = std::chrono::high_resolution_clock::now();
				seed = primer + std::chrono::duration_cast<std::chrono::nanoseconds>(t.time_since_epoch()).count();
			}
			ibe.seed(seed);

			for (size_t count = 0; count < randomSize; count++)
			{
				data.push_back(ibe());
			}
			data.shrink_to_fit();
			// get a new primer for the next time this method is called
			primer += ibe();

			return;
		}


		std::string to_hex_string(const VBytes &v, bool upper_case = true)
		{
			std::stringstream ss;

			for (const int i : v)
			{
				ss << std::hex << std::setfill('0') << std::setw(2) << (upper_case ? std::uppercase : std::nouppercase) << i;
			}

			return ss.str();
		}


		VBytes from_hex_string(const std::string &str)
		{
			if (str.size() % 2)
			{
				/// @throw std::length_error Throws if the string has an odd length.
				throw std::length_error("string has an odd length of " + std::to_string(str.size()) + " bytes");
			}

			if (str.find_first_not_of("0123456789ABCDEFabcdef") != std::string::npos)
			{
				/// @throw std::invalid_argument Throws if the string contains non-hex characters.
				throw std::invalid_argument("string contains non-hex characters");
			}

			VBytes v;
			for (size_t pos = 0; pos < str.size(); pos += 2)
			{
				std::stringstream ss;
				ss << std::hex << str.substr(pos, 2);
				int tmp;
				ss >> tmp;
				v.push_back(tmp);
			}

			return v;
		}


		std::string cbc_encrypt(const std::string &str, const VBytes &key, const VBytes &iv)
		{
			VBytes input(str.begin(), str.end());

			const VBytes v = cbc_encrypt(input, key, iv);

			const std::string output(v.begin(), v.end());

			return output;
		}
		std::string cbc_encrypt_hex(const std::string &str, const VBytes &key, const VBytes &iv)
		{
			VBytes input(str.begin(), str.end());

			const VBytes v = cbc_encrypt(input, key, iv);
			return to_hex_string(v);
		}


		VBytes cbc_encrypt(const VBytes &input, const VBytes &key, const VBytes &iv)
		{
			validate_key_and_iv(key, iv);

			// PKCS #7 padding for AES 16-byte blocks meaning we must add between 1 and 16 bytes of padding
			const size_t original_len	= input.size();
			const size_t padding_len	= 16 - (original_len % 16);
			const char c				= (char)padding_len;

			VBytes v(input);
			v.insert(v.end(), padding_len, c);

			VBytes output(v.size(), ' ');

			uint8_t *ptr1 = reinterpret_cast<uint8_t*>(output.data());
			uint8_t *ptr2 = reinterpret_cast<uint8_t*>(v.data());

			AES_CBC_encrypt_buffer(ptr1, ptr2, v.size(), key.data(), iv.data());

			return output;
		}


		std::string cbc_decrypt(const std::string &str, const VBytes &key, const VBytes &iv)
		{
			VBytes input(str.begin(), str.end());

			const VBytes v = cbc_decrypt(input, key, iv);

			const std::string output(v.begin(), v.end());

			return output;
		}

		std::string cbc_decrypt_hex(const std::string &str, const VBytes &key, const VBytes &iv)
		{
			VBytes input = from_hex_string(str);
			const VBytes v = cbc_decrypt(input, key, iv);
			const std::string output(v.begin(), v.end());

			return output;
		}

		VBytes cbc_decrypt(const VBytes &input, const VBytes &key, const VBytes &iv)
		{
			validate_key_and_iv(key, iv);

			// In the smallest case, a 0-byte buffer would have received 16 bytes of
			// PKCS padding during the encryption phase.  In all cases, the final size
			// must be a multiple of 16 bytes.
			const size_t input_len = input.size();
			if (input_len < 16 || (input_len % 16) != 0)
			{
				/// @throw std::length_error Throws if the buffer to be decrypted has an invalid length.
				throw std::length_error("decryption length of " + std::to_string(input_len) + " bytes is invalid");
			}

			VBytes v(input);
			VBytes output(v.size(), ' ');

			uint8_t *ptr1 = reinterpret_cast<uint8_t*>(output.data());
			uint8_t *ptr2 = reinterpret_cast<uint8_t*>(v.data());

			AES_CBC_decrypt_buffer(ptr1, ptr2, v.size(), key.data(), iv.data());

			// remove the PKCS padding bytes
			const size_t output_len = output.size();
			const char c = output[output_len - 1];
			const size_t padding_len = (size_t)c;

			// AES blocks are 16 bytes in length.  Padding is to ensure we have the
			// exact required number of bytes.  Thus, the padding must be between
			// 1 and 16 bytes to ensure we have exactly 1 AES block.
			if (padding_len < 1 || padding_len > 16)
			{
				/// @throw std::length_error Throws if the PKCS padding after decryption is invalid.
				throw std::length_error("after decrypting vector, PKCS padding has an invalid length of " + std::to_string(padding_len) + " bytes");
			}

			output.resize(output_len - padding_len);

			return output;
		}
	};
}
#endif
