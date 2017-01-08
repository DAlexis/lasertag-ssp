#include "gtest/gtest.h"

#include "../ssp/Inc/ssp-internal.h"

#include <cstring>
#include <iostream>

extern "C" {
	uint8_t ssp_crc8(uint8_t *pcBlock, uint16_t len);
	uint8_t ssp_crc8_seed(uint8_t seed, uint8_t *pcBlock, uint16_t len);
	uint8_t package_crc8(SSP_Header* header, uint8_t *argument);
}

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

TEST(Random, Uniformity)
{
	size_t q1 = 0, q2 = 0, q3 = 0, q4 = 0;
	size_t count = 2000;
	for (size_t i=0; i<count; i++)
	{
		uint32_t r = ssp_random();
		if (r < UINT16_MAX / 4)
			q1++;
		else if (r < UINT16_MAX / 2)
			q2++;
		else if (r < UINT16_MAX * 3 / 4)
			q3++;
		else
			q4++;
	}
	ASSERT_NEAR(q1, q2, count * 0.02);
	ASSERT_NEAR(q1, q3, count * 0.02);
	ASSERT_NEAR(q1, q4, count * 0.02);
}

TEST(Random, Period)
{
	uint16_t start = ssp_random();
	size_t counter = 1;
	while(start != ssp_random())
		counter++;
	ASSERT_GT(counter, 65534);
}
