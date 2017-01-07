#include "gtest/gtest.h"

#include "../ssp/Inc/ssp-internal.h"

#include <cstring>


TEST(CRC8Test, Concatenation)
{
	uint8_t buf1[] = "Hello";
	uint8_t buf2[] = "world";
	uint8_t buf3[100];
	memcpy(buf3, buf1, sizeof(buf1));
	memcpy(buf3+sizeof(buf1), buf2, sizeof(buf2));
	uint8_t whole = ssp_crc8(buf3, sizeof(buf1) + sizeof(buf2));

	uint8_t part = ssp_crc8(buf1, sizeof(buf1));
	part = ssp_crc8_seed(part, buf2, sizeof(buf2));
	ASSERT_EQ(whole, part);
}

/*
TEST(CRC8Test, DecodeEncode)
{
	SSP_Package package;
	char text[] = "Test";
	package.header.command = SSP_S2M_DEBUG;
	package.header.sender = 123;
	package.header.size = 5;
	package.header.target = 321;
	package.argument = (uint8_t*) text;

	package_crc8(&package.header, package.argument);
	uint8_t tmp = package.header.crc8;
	package_crc8(&package.header, package.argument);
	ASSERT_EQ(tmp, package.header.crc8);

	uint8_t buffer[100];
	memcpy(buffer, &package.header, sizeof(SSP_Header));
	memcpy(buffer+sizeof(SSP_Header), text, package.header.size);
	tmp = ssp_crc8(buffer + sizeof(SSP_Checksum), sizeof(SSP_Header) + package.header.size - sizeof(SSP_Checksum));
	ASSERT_EQ(tmp, package.header.crc8);
}
*/
